/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>
#include <SDL_ttf.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/settingsmanager.h"
#include "util/exception.h"
#include "util/logger.h"
#include "util/logger.h"
#include "util/time/timemanager.h"
#include "audio/audiomanager.h"
#include "gui/console/console.h"
#include "gui/guimanager.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/vfssourcefactory.h"
#ifdef HAVE_ZIP
#include "vfs/zip/zipprovider.h"
#endif
#include "eventchannel/manager/eventmanager.h"
#include "video/imagepool.h"
#include "video/animationpool.h"
#include "video/renderbackend.h"
#ifdef HAVE_OPENGL
#include "video/opengl/renderbackendopengl.h"
#include "gui/base/opengl/opengl_gui_graphics.h"
#endif
#include "gui/base/sdl/sdl_gui_graphics.h"
#include "video/sdl/renderbackendsdl.h"
#include "loaders/native/video_loaders/subimage_provider.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
//#include "loaders/fallout/model_loaders/dat1.h"
//#include "loaders/fallout/model_loaders/dat2.h"
#include "model/model.h"
#include "pathfinder/linearpather/linearpather.h"
#include "pathfinder/routepather/routepather.h"

#include "view/view.h"
#include "view/renderers/camerazonerenderer.h"
#include "view/renderers/gridrenderer.h"
#include "view/renderers/instancerenderer.h"
#include "engine.h"

#ifdef USE_COCOA
#include <dlfcn.h>

int main(int argc, char **argv)
{
    return 0;
}
#endif

static const std::string SETTINGS_FILE_NAME = "fife.config";

namespace FIFE {
	static Logger _log(LM_CONTROLLER);

	Engine::Engine(bool use_miniwindow):
		m_renderbackend(0),
		m_guimanager(0),
		m_eventmanager(0),
		m_audiomanager(0),
		m_timemanager(0),
		m_settingsmanager(0),
		m_imagepool(0),
		m_animpool(0),
		m_vfs_sourcefactory(0),
		m_vfs(0),
		m_model(0),
		m_gui_graphics(0),
		m_view(0),
		m_logmanager(0),
		m_use_miniwindow(use_miniwindow) {

#ifdef USE_COCOA
		// The next lines ensure that Cocoa is initialzed correctly.
		// This is needed for SDL to function properly on MAC OS X.
		void* cocoa_lib; 
		cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY ); 
		void (*nsappload)(void); 
		nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad"); 
		nsappload(); 
#endif
		init();
	}

	Engine::~Engine() {
		delete m_view;
		delete m_model;
		delete m_audiomanager;
		delete m_guimanager;
		delete m_gui_graphics;
		delete m_eventmanager;

		m_renderbackend->deinit();
		delete m_renderbackend;

		// Note the dependancy between image and animation pools
		// as animations reference images they have to be deleted
		// before clearing the image pool.
		delete m_animpool;
		delete m_imagepool;

		delete m_vfs;
		delete m_vfs_sourcefactory;

		delete m_timemanager;
		// DO override not pre-existing config.
		m_settingsmanager->saveSettings(SETTINGS_FILE_NAME,true);
		delete m_settingsmanager;

		TTF_Quit();
		SDL_Quit();
	}

	void Engine::init() {
		m_logmanager = LogManager::instance();
		m_settingsmanager = new SettingsManager();
		m_settingsmanager->loadSettings(SETTINGS_FILE_NAME);

		// If failed to init SDL throw exception.
		if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
			throw SDLException(SDL_GetError());
		}

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		TTF_Init();

		m_eventmanager = new EventManager();

		m_imagepool = new ImagePool();
		m_animpool = new AnimationPool();
		m_imagepool->addResourceProvider(new SubImageProvider());
		m_imagepool->addResourceProvider(new ImageProvider());
		m_animpool->addResourceProvider(new AnimationProvider(m_imagepool));

		m_guimanager = new GUIManager(m_eventmanager, *m_imagepool);
		m_eventmanager->addSdlEventListener(m_guimanager);
		// keep guimanager as the first key listener so that it can evaluate guichan hits
		m_eventmanager->addKeyListener(m_guimanager);
		// keep guimanager as the first mouse listener so that it can evaluate guichan hits
		m_eventmanager->addMouseListener(m_guimanager);

		// Select the render backend.
		std::string rbackend = m_settingsmanager->read<std::string>("RenderBackend", "SDL");
		if (rbackend == "SDL") {
			m_renderbackend = new RenderBackendSDL();
			FL_LOG(_log, "SDL Render backend created");
		} else {
#ifdef HAVE_OPENGL
			m_renderbackend = new RenderBackendOpenGL();
			FL_LOG(_log, "OpenGL Render backend created");
#else
			m_renderbackend = new RenderBackendSDL();
			// Remember  the choice so we pick the right graphics class.
			rbackend = "SDL";
			FL_WARN(_log, "Tried to select OpenGL, even though it is not compiled into the engine. Falling back to SDL Render backend");
#endif
		}
		m_renderbackend->init();

		m_timemanager = new TimeManager();
		m_vfs_sourcefactory = new VFSSourceFactory();
		m_vfs = new VFS();

		m_vfs->addSource(new VFSHostSystem());
#ifdef HAVE_ZIP
		m_vfs_sourcefactory->addProvider( new ZipProvider() );
#endif
		//m_vfs_sourcefactory->addProvider(ProviderDAT2());
		//m_vfs_sourcefactory->addProvider(ProviderDAT1());

		m_audiomanager = new AudioManager();
		int volume = m_settingsmanager->read<int>("InitialVolume", 5);
		if (volume > 10) {
			volume = 10;
		} else if (volume < 0) {
			volume = 0;
		}
		m_audiomanager->setVolume(static_cast<float>(volume) / 10);

		unsigned int swidth = m_settingsmanager->read("ScreenWidth", 800);
		unsigned int sheight = m_settingsmanager->read("ScreenHeight", 600);
		if (m_use_miniwindow) {
			swidth = 1;
			sheight = 1;
		}
		unsigned char bitsPerPixel = m_settingsmanager->read("BitsPerPixel", 0);
		bool fullscreen = m_settingsmanager->read("FullScreen", false);

		m_renderbackend->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);
#ifdef HAVE_OPENGL
		if( rbackend != "SDL" ) {
			m_gui_graphics = new OpenGLGuiGraphics(*m_imagepool);
		}
#endif
		if( rbackend == "SDL" ) {
			m_gui_graphics = new SdlGuiGraphics(*m_imagepool);
		}
		m_guimanager->init(m_gui_graphics, swidth, sheight);
		SDL_EnableUNICODE(1);

		m_model = new Model();
		m_model->addPather(new LinearPather());
		m_model->addPather(new RoutePather());
		
		m_view = new View();
		m_view->addRenderer(new CameraZoneRenderer(m_renderbackend, m_imagepool));
		m_view->addRenderer(new GridRenderer(m_renderbackend));
		m_view->addRenderer(new InstanceRenderer(m_renderbackend, m_imagepool, m_animpool));
	}

	void Engine::initializePumping() {
		m_eventmanager->processEvents();
		m_renderbackend->startFrame();
	}

	void Engine::pump() {
		m_timemanager->update();
		m_model->update();
		m_view->update();
		m_guimanager->turn();
		m_renderbackend->endFrame();
		SDL_Delay(1);
		m_eventmanager->processEvents();
		m_renderbackend->startFrame();
	}

	void Engine::finalizePumping() {
		m_timemanager->update();
		m_guimanager->turn();
		m_renderbackend->endFrame();
	}
}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
