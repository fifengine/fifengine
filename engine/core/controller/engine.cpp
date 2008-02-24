/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/exception.h"
#include "util/logger.h"
#include "util/logger.h"
#include "util/time/timemanager.h"
#include "audio/soundmanager.h"
#include "gui/console/console.h"
#include "gui/guimanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/directoryprovider.h"
#ifdef HAVE_ZIP
#include "vfs/zip/zipprovider.h"
#endif
#include "eventchannel/eventmanager.h"
#include "video/imagepool.h"
#include "video/animationpool.h"
#include "video/renderbackend.h"
#include "video/cursor.h"
#ifdef HAVE_OPENGL
#include "video/opengl/renderbackendopengl.h"
#include "gui/base/opengl/opengl_gui_graphics.h"
#endif
#include "gui/base/sdl/sdl_gui_graphics.h"
#include "gui/base/gui_font.h"
#include "video/sdl/renderbackendsdl.h"
#include "video/fonts/abstractfont.h"
#include "loaders/native/video_loaders/subimage_loader.h"
#include "loaders/native/video_loaders/image_loader.h"
#include "loaders/native/video_loaders/animation_loader.h"
//#include "loaders/fallout/model_loaders/dat1.h"
//#include "loaders/fallout/model_loaders/dat2.h"
#include "model/model.h"
#include "pathfinder/linearpather/linearpather.h"
#include "pathfinder/routepather/routepather.h"

#include "view/view.h"
#include "view/renderers/camerazonerenderer.h"
#include "view/renderers/quadtreerenderer.h"
#include "view/renderers/gridrenderer.h"
#include "view/renderers/instancerenderer.h"
#include "view/renderers/coordinaterenderer.h"
#include "view/renderers/floatingtextrenderer.h"
#include "view/renderers/cellselectionrenderer.h"
#include "view/renderers/blockinginforenderer.h"
#include "engine.h"

#ifdef USE_COCOA
#include <dlfcn.h>

int main(int argc, char **argv)
{
    return 0;
}
#endif

namespace FIFE {
	static Logger _log(LM_CONTROLLER);

	Engine::Engine():
		m_renderbackend(0),
		m_guimanager(0),
		m_eventmanager(0),
		m_soundmanager(0),
		m_timemanager(0),
		m_imagepool(0),
		m_animpool(0),
		m_vfs(0),
		m_model(0),
		m_gui_graphics(0),
		m_view(0),
		m_logmanager(0),
		m_cursor(0),
		m_settings() {
#ifdef USE_COCOA
		// The next lines ensure that Cocoa is initialzed correctly.
		// This is needed for SDL to function properly on MAC OS X.
		void* cocoa_lib;
		cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
		void (*nsappload)(void);
		nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad");
		nsappload();
#endif
		preInit();
	}

	EngineSettings& Engine::getSettings() {
		return m_settings;
	}

	void Engine::preInit() {
		m_logmanager = LogManager::instance();

		FL_LOG(_log, "================== Engine pre-init start =================");
		m_timemanager = new TimeManager();
		FL_LOG(_log, "Time manager created");

		FL_LOG(_log, "Creating VFS");
		m_vfs = new VFS();

		FL_LOG(_log, "Adding root directory to VFS");
		m_vfs->addSource( new VFSDirectory(m_vfs) );
		m_vfs->addProvider( new DirectoryProvider() );
#ifdef HAVE_ZIP
		FL_LOG(_log, "Adding zip provider to VFS");
		m_vfs->addProvider( new ZipProvider() );
#endif
		//m_vfs->addProvider(ProviderDAT2());
		//m_vfs->addProvider(ProviderDAT1());
		FL_LOG(_log, "Engine pre-init done");
	}

	void Engine::init() throw(NotSet) {
		FL_LOG(_log, "Engine initialize start");
		m_settings.validate();
		FL_LOG(_log, "Engine settings validated");

		// If failed to init SDL throw exception.
		if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {
			throw SDLException(SDL_GetError());
		}

		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		TTF_Init();

		FL_LOG(_log, "Creating event manager");
		m_eventmanager = new EventManager();

		FL_LOG(_log, "Creating pools");
		m_imagepool = new ImagePool();
		m_animpool = new AnimationPool();
		m_imagepool->addResourceLoader(new SubImageLoader());
		m_imagepool->addResourceLoader(new ImageLoader(m_vfs));
		m_animpool->addResourceLoader(new AnimationLoader(m_imagepool));

		FL_LOG(_log, "Creating render backend");
		std::string rbackend(m_settings.getRenderBackend());
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
		FL_LOG(_log, "Initializing render backend");
		m_renderbackend->init();

		FL_LOG(_log, "Creating main screen");
		m_renderbackend->createMainScreen(
			m_settings.getScreenWidth(),
			m_settings.getScreenHeight(),
			static_cast<unsigned char>(m_settings.getBitsPerPixel()),
			m_settings.isFullScreen());
		FL_LOG(_log, "Main screen created");

#ifdef HAVE_OPENGL
		if( rbackend != "SDL" ) {
			m_gui_graphics = new OpenGLGuiGraphics(*m_imagepool);
		}
#endif
		if( rbackend == "SDL" ) {
			m_gui_graphics = new SdlGuiGraphics(*m_imagepool);
		}
		FL_LOG(_log, "Constructing GUI manager");
		m_guimanager = new GUIManager(m_eventmanager, *m_imagepool);
		m_eventmanager->addSdlEventListener(m_guimanager);
		// keep guimanager as the first key listener so that it can evaluate guichan hits
		m_eventmanager->addKeyListener(m_guimanager);
		// keep guimanager as the first mouse listener so that it can evaluate guichan hits
		m_eventmanager->addMouseListener(m_guimanager);
		FL_LOG(_log, "Events bind to GUI manager");

		FL_LOG(_log, "Creating default font");
		m_defaultfont = m_guimanager->setDefaultFont(
			m_settings.getDefaultFontPath(),
			m_settings.getDefaultFontSize(),
			m_settings.getDefaultFontGlyphs());
		FL_LOG(_log, "Initializing GUI manager");
		m_guimanager->init(m_gui_graphics, m_settings.getScreenWidth(), m_settings.getScreenHeight());
		FL_LOG(_log, "GUI manager initialized");
		SDL_EnableUNICODE(1);

		FL_LOG(_log, "Creating sound manager");
		m_soundmanager = new SoundManager();
		m_soundmanager->setVolume(static_cast<float>(m_settings.getInitialVolume()) / 10);

		FL_LOG(_log, "Creating model");
		m_model = new Model();
		m_model->setTriggerController(m_eventmanager);
		FL_LOG(_log, "Adding pathers to model");
		m_model->adoptPather(new LinearPather());
		m_model->adoptPather(new RoutePather());

		FL_LOG(_log, "Creating view");
		m_view = new View(m_renderbackend, m_imagepool, m_animpool);
		FL_LOG(_log, "Creating renderers to view");
		m_view->addRenderer(new CameraZoneRenderer(m_renderbackend, 0, m_imagepool));
		m_view->addRenderer(new InstanceRenderer(m_renderbackend, 10, m_imagepool, m_animpool));
		m_view->addRenderer(new GridRenderer(m_renderbackend, 20));
		m_view->addRenderer(new CellSelectionRenderer(m_renderbackend, 30));
		m_view->addRenderer(new BlockingInfoRenderer(m_renderbackend, 40));
		m_view->addRenderer(new FloatingTextRenderer(m_renderbackend, 50, dynamic_cast<AbstractFont*>(m_defaultfont)));
		m_view->addRenderer(new QuadTreeRenderer(m_renderbackend, 60));
		m_view->addRenderer(new CoordinateRenderer(m_renderbackend, 70, dynamic_cast<AbstractFont*>(m_defaultfont)));
		m_cursor = new Cursor(m_imagepool, m_animpool, m_renderbackend);
		FL_LOG(_log, "Engine intialized");
	}

	Engine::~Engine() {
		FL_LOG(_log, "Destructing engine");
 		delete m_cursor;
 		delete m_view;
		delete m_model;
		delete m_soundmanager;
		delete m_guimanager;
		delete m_gui_graphics;

		m_renderbackend->deinit();
		delete m_renderbackend;

		// Note the dependancy between image and animation pools
		// as animations reference images they have to be deleted
		// before clearing the image pool.
		delete m_animpool;
		delete m_imagepool;
		delete m_eventmanager;

		delete m_vfs;

		delete m_timemanager;

		TTF_Quit();
		SDL_Quit();
		FL_LOG(_log, "================== Engine destructed ==================");
		//delete m_logmanager;
	}
	void Engine::initializePumping() {
		m_eventmanager->processEvents();
	}

	void Engine::pump() {
		m_renderbackend->startFrame();
		m_timemanager->update();
		m_model->update();
		m_view->update();
		m_guimanager->turn();
		m_cursor->draw();
		m_renderbackend->endFrame();
		SDL_Delay(1);
		m_eventmanager->processEvents();
		m_renderbackend->startFrame();
		
	}

	void Engine::finalizePumping() {
		// nothing here at the moment..
	}
}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
