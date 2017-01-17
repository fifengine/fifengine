/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <iostream>
#include <algorithm>

// 3rd party library includes
#include <SDL.h>
#include <SDL_ttf.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/time/timemanager.h"
#include "audio/soundmanager.h"
#include "gui/guimanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/directoryprovider.h"
#ifdef HAVE_ZIP
#include "vfs/zip/zipprovider.h"
#endif
#include "eventchannel/eventmanager.h"
#include "video/imagemanager.h"
#include "video/animationmanager.h"
#include "audio/soundclipmanager.h"
#include "video/renderbackend.h"
#include "video/cursor.h"
#include "video/devicecaps.h"
#ifdef HAVE_OPENGL
#include "video/opengl/fife_opengl.h"
#include "video/opengl/renderbackendopengl.h"
#endif
#include "video/sdl/renderbackendsdl.h"
#include "loaders/native/video/imageloader.h"
#include "loaders/native/audio/ogg_loader.h"
#include "model/model.h"
#include "pathfinder/routepather/routepather.h"
#include "model/metamodel/grids/hexgrid.h"
#include "model/metamodel/grids/squaregrid.h"
#include "view/renderers/quadtreerenderer.h"
#include "view/renderers/gridrenderer.h"
#include "view/renderers/instancerenderer.h"
#include "view/renderers/coordinaterenderer.h"
#include "view/renderers/floatingtextrenderer.h"
#include "view/renderers/cellselectionrenderer.h"
#include "view/renderers/blockinginforenderer.h"
#include "view/renderers/genericrenderer.h"
#include "view/renderers/targetrenderer.h"
#include "view/renderers/lightrenderer.h"
#include "view/renderers/offrenderer.h"
#include "view/renderers/cellrenderer.h"
#include "video/image.h"
#include "engine.h"
#include "version.h"

#ifdef USE_COCOA

#include <objc/message.h>
#include <dlfcn.h>

int32_t main(int32_t argc, char **argv)
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
		m_imagemanager(0),
		m_animationmanager(0),
		m_soundclipmanager(0),
		m_vfs(0),
		m_model(0),
		m_logmanager(0),
		m_cursor(0),
		m_destroyed(false),
		m_settings(),
		m_devcaps(),
		m_offrenderer(0),
		m_targetrenderer(0),
		m_changelisteners() {
#ifdef USE_COCOA
		// The next lines ensure that Cocoa is initialzed correctly.
		// This is needed for SDL to function properly on MAC OS X.
		void* cocoa_lib;
		cocoa_lib = dlopen( "/System/Library/Frameworks/Cocoa.framework/Cocoa", RTLD_LAZY );
		void (*nsappload)(void);
		nsappload = (void(*)()) dlsym( cocoa_lib, "NSApplicationLoad");
		nsappload();

		// Create an autorelease pool, so autoreleased SDL objects don't leak.
#ifdef OSX_109
		Class NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
		m_autoreleasePool = class_createInstance(NSAutoreleasePool, 0);
#else
		objc_object *NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
		m_autoreleasePool =
			objc_msgSend(NSAutoreleasePool, sel_registerName("new"));
#endif
#endif
		m_logmanager = LogManager::instance();
	}

	EngineSettings& Engine::getSettings() {
		return m_settings;
	}

	const DeviceCaps& Engine::getDeviceCaps() const {
		return m_devcaps;
	}

	void Engine::changeScreenMode(const ScreenMode& mode){
		m_cursor->invalidate();

		m_imagemanager->invalidateAll();

		m_renderbackend->setScreenMode(mode);

		if (m_guimanager) {
			m_guimanager->resizeTopContainer(0,0,mode.getWidth(), mode.getHeight());
		}

		std::vector<IEngineChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onScreenModeChanged(mode);
			++i;
		}
	}

	void Engine::init() {
		m_destroyed = false;

		FL_LOG(_log, LMsg("Fifengine v") << FIFE::getVersion());
		FL_LOG(_log, "================== Engine initialize start =================");
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

		// If failed to init SDL throw exception.
		if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {
			throw SDLException(SDL_GetError());
		}

		TTF_Init();

		FL_LOG(_log, "Creating event manager");
		m_eventmanager = new EventManager();
		m_eventmanager->setMouseSensitivity(m_settings.getMouseSensitivity());
		m_eventmanager->setMouseAccelerationEnabled(m_settings.isMouseAccelerationEnabled());

		FL_LOG(_log, "Creating resource managers");

		m_imagemanager = new ImageManager();
		m_animationmanager = new AnimationManager();
		m_soundclipmanager = new SoundClipManager();

		FL_LOG(_log, "Creating render backend");
		std::string rbackend(m_settings.getRenderBackend());
		if (rbackend == "SDL") {
			m_renderbackend = new RenderBackendSDL(m_settings.getColorKey());
			FL_LOG(_log, "SDL Render backend created");
		} else {
#ifdef HAVE_OPENGL
			m_renderbackend = new RenderBackendOpenGL(m_settings.getColorKey());
			FL_LOG(_log, "OpenGL Render backend created");
#else
			m_renderbackend = new RenderBackendSDL(m_settings.getColorKey());
			// Remember  the choice so we pick the right graphics class.
			rbackend = "SDL";
			FL_WARN(_log, "Tried to select OpenGL, even though it is not compiled into the engine. Falling back to SDL Render backend");
#endif
		}
		FL_LOG(_log, "Initializing render backend");
		m_renderbackend->setColorKeyEnabled(m_settings.isColorKeyEnabled());
		// we always set this to false
		//m_renderbackend->setAlphaOptimizerEnabled(false);
		m_renderbackend->setImageCompressingEnabled(m_settings.isGLCompressImages());
		m_renderbackend->setFramebufferEnabled(m_settings.isGLUseFramebuffer());
		m_renderbackend->setNPOTEnabled(m_settings.isGLUseNPOT());
		m_renderbackend->setTextureFiltering(m_settings.getGLTextureFiltering());
		m_renderbackend->setMipmappingEnabled(m_settings.isGLUseMipmapping());
		m_renderbackend->setMonochromeEnabled(m_settings.isGLUseMonochrome());
		m_renderbackend->setDepthBufferEnabled(m_settings.isGLUseDepthBuffer());
		m_renderbackend->setAlphaTestValue(m_settings.getGLAlphaTestValue());
		m_renderbackend->setVSyncEnabled(m_settings.isVSync());
		if (m_settings.isFrameLimitEnabled()) {
			m_renderbackend->setFrameLimitEnabled(true);
			m_renderbackend->setFrameLimit(m_settings.getFrameLimit());
		}

		std::string driver = m_settings.getVideoDriver();
		if (driver != ""){
			std::vector<std::string> drivers = m_devcaps.getAvailableVideoDrivers();
			if (std::find (drivers.begin(), drivers.end(), driver) == drivers.end()) {
				FL_WARN(_log, "Selected video driver is not supported for your Operating System!  Reverting to default driver.");
				driver = "";
			}
			m_devcaps.setVideoDriverName(driver);
		}
		// init backend with selected video driver or default
		m_renderbackend->init(driver);

		// in case of SDL we use this to create the SDL_Renderer
		driver = m_settings.getSDLDriver();
		if (driver != ""){
			std::vector<std::string> drivers = m_devcaps.getAvailableRenderDrivers();
			if (std::find (drivers.begin(), drivers.end(), driver) == drivers.end()) {
				FL_WARN(_log, "Selected render driver is not supported for your Operating System!  Reverting to default driver.");
				driver = "";
			}
			m_devcaps.setRenderDriverName(driver);
		}

		FL_LOG(_log, "Querying device capabilities");
		m_devcaps.fillDeviceCaps();

		uint16_t bpp = m_settings.getBitsPerPixel();

		m_screenMode = m_devcaps.getNearestScreenMode(
			m_settings.getScreenWidth(),
			m_settings.getScreenHeight(),
			bpp,
			rbackend,
			m_settings.isFullScreen(),
			m_settings.getRefreshRate(),
			m_settings.getDisplay());

		FL_LOG(_log, "Creating main screen");
		m_renderbackend->createMainScreen(
			m_screenMode,
			m_settings.getWindowTitle(),
			m_settings.getWindowIcon());
		FL_LOG(_log, "Main screen created");

#ifdef HAVE_OPENGL
		if (m_settings.getLightingModel() != 0) {
			m_renderbackend->setLightingModel(m_settings.getLightingModel());
		}

#endif
		FL_LOG(_log, "Creating sound manager");
		m_soundmanager = new SoundManager();
		m_soundmanager->setVolume(static_cast<float>(m_settings.getInitialVolume()) / 10);

		FL_LOG(_log, "Creating renderers");
		m_offrenderer = new OffRenderer(m_renderbackend);
		m_targetrenderer = new TargetRenderer(m_renderbackend);
		m_renderers.push_back(new InstanceRenderer(m_renderbackend, 10));
		m_renderers.push_back(new GridRenderer(m_renderbackend, 20));
		m_renderers.push_back(new CellSelectionRenderer(m_renderbackend, 30));
		m_renderers.push_back(new BlockingInfoRenderer(m_renderbackend, 40));
		m_renderers.push_back(new FloatingTextRenderer(m_renderbackend, 50));
		m_renderers.push_back(new QuadTreeRenderer(m_renderbackend, 60));
		m_renderers.push_back(new CoordinateRenderer(m_renderbackend, 70));
		m_renderers.push_back(new GenericRenderer(m_renderbackend, 80));
		m_renderers.push_back(new LightRenderer(m_renderbackend, 90));
		m_renderers.push_back(new CellRenderer(m_renderbackend, 100));

		FL_LOG(_log, "Creating model");
		m_model = new Model(m_renderbackend, m_renderers);
		FL_LOG(_log, "Adding pathers to model");
		m_model->adoptPather(new RoutePather());
		FL_LOG(_log, "Adding grid prototypes to model");
		m_model->adoptCellGrid(new SquareGrid());
		m_model->adoptCellGrid(new HexGrid());

		m_cursor = new Cursor(m_renderbackend);
		FL_LOG(_log, "Engine initialized");
	}

	Engine::~Engine() {
		if( !m_destroyed ) {
			destroy();
		}
	}

	void Engine::destroy() {
		FL_LOG(_log, "Destructing engine");
 		delete m_cursor;
		delete m_model;
		delete m_soundmanager;
		delete m_guimanager;

		delete m_animationmanager;
		delete m_imagemanager;
		delete m_soundclipmanager;
		delete m_eventmanager;

		// properly remove all the renderers created during init
		delete m_offrenderer;
		delete m_targetrenderer;
		std::vector<RendererBase*>::iterator rendererIter = m_renderers.begin();
		for ( ; rendererIter != m_renderers.end(); ++rendererIter)
		{
			delete *rendererIter;
		}
		m_renderers.clear();

		delete m_renderbackend;
		delete m_vfs;
		delete m_timemanager;

		TTF_Quit();
		SDL_Quit();

#ifdef USE_COCOA
		objc_msgSend(m_autoreleasePool, sel_registerName("release"));
#endif

		FL_LOG(_log, "================== Engine destructed ==================");
		m_destroyed = true;
		//delete m_logmanager;
	}
	void Engine::initializePumping() {
		m_eventmanager->processEvents();
	}

	void Engine::pump() {
		m_renderbackend->startFrame();
		m_eventmanager->processEvents();
		m_timemanager->update();

        m_renderbackend->clearBackBuffer();
		m_targetrenderer->render();
		if (m_model->getActiveCameraCount() == 0) {
			m_offrenderer->render();
		} else {
			m_model->update();
		}

		if (m_guimanager) {
			m_guimanager->turn();
		}

		m_cursor->draw();
		m_renderbackend->endFrame();
	}

	void Engine::finalizePumping() {
		// nothing here at the moment..
	}

	void Engine::addChangeListener(IEngineChangeListener* listener) {
		m_changelisteners.push_back(listener);
	}

	void Engine::removeChangeListener(IEngineChangeListener* listener) {
		std::vector<IEngineChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			if ((*i) == listener) {
				m_changelisteners.erase(i);
				return;
			}
			++i;
		}
	}
}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
