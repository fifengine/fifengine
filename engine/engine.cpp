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
#include "util/log.h"
#include "util/debugutils.h"
#include "util/time/timemanager.h"
#include "audio/audiomanager.h"
#include "gui/console.h"
/*
#include "loaders/native/video_loaders/complexanimation_provider.h"
#include "loaders/fallout/video_loaders/frm_provider.h"
#include "loaders/native/video_loaders/animation_provider.h"
#include "loaders/native/video_loaders/image_provider.h"
#include "loaders/native/video_loaders/subimage_provider.h"
*/
#include "eventchannel/manager/eventmanager.h"
#include "gui/guimanager.h"
#include "video/imagecache.h"
#include "video/renderbackend.h"
#include "video/renderbackends/opengl/renderbackendopengl.h"
#include "video/renderbackends/sdl/renderbackendsdl.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/vfssourcefactory.h"
#ifdef HAVE_ZIP
#include "vfs/zip/zipprovider.h"
#endif

#include "engine.h"

static const std::string SETTINGS_FILE_NAME = "fife.config";

namespace FIFE {

	Engine::Engine() {
		init();
	}

	Engine::~Engine() {
		delete audio::Manager::instance();

		delete GUIManager::instance();

		delete EventManager::instance();

		RenderBackend::instance()->deinit();
		delete RenderBackend::instance();
		delete ImageCache::instance();

		delete VFS::instance();
		delete VFSSourceFactory::instance();

		delete TimeManager::instance();
		// DO override not pre-existing config.
		SettingsManager::instance()->saveSettings(SETTINGS_FILE_NAME,true);
		delete SettingsManager::instance();

		TTF_Quit();
		SDL_Quit();
	}

	void Engine::init() {
		FIFE::SettingsManager* settings = new SettingsManager();
		settings->loadSettings(SETTINGS_FILE_NAME);

		// If failed to init SDL throw exception.
		if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
			throw SDLException(SDL_GetError());
		}

		// This may be a good idea; the guichan examples do it, I don't know why...
		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		TTF_Init();

#ifdef HAVE_MOVIE
		av_register_all();
#endif

		m_eventmanager = new EventManager();
		m_guimanager = new GUIManager();
		m_eventmanager->addSdlEventListener(m_guimanager);
		// keep guimanager as the first key listener so that it can evaluate guichan hits
		m_eventmanager->addKeyListener(m_guimanager);
		// keep guimanager as the first mouse listener so that it can evaluate guichan hits
		m_eventmanager->addMouseListener(m_guimanager);

		// Select the render backend.
		std::string rbackend = settings->read<std::string>("RenderBackend", "SDL");
		if (rbackend == "SDL") {
			m_renderbackend = new RenderBackendSDL();
			std::cout << "SDL Render backend created" << std::endl;
		} else {
			m_renderbackend = new RenderBackendOpenGL();
			std::cout << "OpenGL Render backend created" << std::endl;
		}
		m_renderbackend->init();

		m_timemanager = new TimeManager();
		new VFSSourceFactory();
		new VFS();

		VFS::instance()->addSource(new VFSHostSystem());
#ifdef HAVE_ZIP
		// FIXME: Change all providers to be loaded here or even better in scripts
		VFSSourceFactory::instance()->addProvider( new zip::ZipProvider() );
#endif

		m_imagecache = new ImageCache();
/*
		m_imagecache->addRenderableProviderConstructor(new RenderableProviderConstructorTempl<
		                                               video::loaders::FRMProvider,
		                                               RenderAble::RT_IMAGE|RenderAble::RT_ANIMATION|RenderAble::RT_UNDEFINED >());

		m_imagecache->addRenderableProviderConstructor(new RenderableProviderConstructorTempl<
		                                               video::loaders::AnimationProvider,
		                                               RenderAble::RT_ANIMATION >());

		m_imagecache->addRenderableProviderConstructor(new RenderableProviderConstructorTempl<
		                                               video::loaders::ComplexAnimationProvider,
		                                               RenderAble::RT_COMPLEX_ANIMATION >());

		m_imagecache->addRenderableProviderConstructor(new RenderableProviderConstructorTempl<
		                                               video::loaders::SubImageProvider,
		                                               RenderAble::RT_SUBIMAGE >());

		m_imagecache->addRenderableProviderConstructor(new RenderableProviderConstructorTempl<
		                                               video::loaders::ImageProvider,
		                                               RenderAble::RT_IMAGE|RenderAble::RT_UNDEFINED >());
*/
		new audio::Manager();
		int volume = settings->read<int>("InitialVolume", 5);
		if (volume > 10) {
			volume = 10;
		} else if (volume < 0) {
			volume = 0;
		}
		audio::Manager::instance()->setVolume(static_cast<float>(volume) / 10);

		unsigned int swidth = settings->read("ScreenWidth", 800);
		unsigned int sheight = settings->read("ScreenHeight", 600);
		unsigned char bitsPerPixel = settings->read("BitsPerPixel", 0);
		bool fullscreen = settings->read("FullScreen", false);

		FIFE::RenderBackend::instance()->createMainScreen(swidth, sheight, bitsPerPixel, fullscreen);
		m_guimanager->init();
		SDL_EnableUNICODE(1);
		m_guimanager->getConsole()->show();
	}

	void Engine::pump() {
		m_eventmanager->processEvents();
		m_renderbackend->startFrame();
		m_timemanager->update();
		m_guimanager->turn();
		m_renderbackend->endFrame();
		m_imagecache->collect();
		SDL_Delay(1);
	}

	audio::Manager* Engine::getAudioManager() {
		return audio::Manager::instance();
	}

	EventManager* Engine::getEventManager() {
		return EventManager::instance();
	}

	VFS* Engine::getVFS() {
		return VFS::instance();
	}

	TimeManager* Engine::getTimeManager() {
		return TimeManager::instance();
	}

	SettingsManager* Engine::getSettingsManager() {
		return SettingsManager::instance();
	}

	GUIManager* Engine::getGuiManager() {
		return m_guimanager;
	}

}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
