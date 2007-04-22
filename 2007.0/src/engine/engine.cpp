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

// 3rd party library includes
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <SDL.h>
#include <SDL_ttf.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "audio/manager.h"
#include "guichan_addon/console.h"
#include "input/manager.h"
#include "map/factory.h"
#include "script/scriptbackendmanager.h"
#include "video/gui/guimanager.h"
#include "video/renderbackend.h"
#include "video/rendermanager.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "vfs/vfssourcefactory.h"

#include "debugutils.h"
#include "engine.h"
#include "events.h"
#include "exception.h"
#include "gamestate.h"
#include "gamestatemanager.h"
#include "imagecache.h"
#include "log.h"
#include "settingsmanager.h"
#include "settingsmanager.h"
#include "timemanager.h"

namespace FIFE {

	Engine::Engine(int argc, char** argv) 
		: m_run(false), m_makeScreenshot(false), m_avgframetime(0) {

		while( --argc ) {
			m_cmdline.push_back( argv[argc] );
		}

		init();
	}

	Engine::~Engine() {
		killSingletons();
		TTF_Quit();
		SDL_Quit();
	}


	void Engine::init() {
		new SettingsManager();

		// If failed to init SDL throw exception.
		if (SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	
			throw SDLException(SDL_GetError());
		}

		// This may be a good idea; the guichan examples do it, I don't know why...
		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		TTF_Init();


		Log::parseCmdLine( m_cmdline );

		new GUIManager();
		new RenderManager();

		// Select the render backend.
		std::string rbackend = SettingsManager::instance()->read<std::string>("RenderBackend", "SDL");
		RenderManager::instance()->select(rbackend);

		new VFSSourceFactory();
		new VFS();

		VFS::instance()->addSource(new VFSHostSystem());

		new input::Manager();
		new ImageCache();
		new map::Factory();
		new audio::Manager();
		new GameStateManager();
		new ScriptBackendManager();
		new TimeManager();
	}

	void Engine::killSingletons() {
		delete GameStateManager::instance();
		delete ScriptBackendManager::instance();
		delete audio::Manager::instance();
		delete map::Factory::instance();
		delete ImageCache::instance();
		delete VFS::instance();
		delete VFSSourceFactory::instance();
		delete RenderManager::instance();
		delete GUIManager::instance();
		delete input::Manager::instance();
		delete SettingsManager::instance();
		delete TimeManager::instance();
	}

	void Engine::start() {
		CScriptEngine()->callFunction("on_engine_start");
		m_run = true;
		mainLoop();
	}

	void Engine::stop() {
		m_run = false;
	}

	void Engine::handleEvent(int event) {
		if (event == Event::QUIT_GAME) {
			stop();
		} else if (event == Event::MAKE_SCREENSHOT) {
			m_makeScreenshot = true;
		} else if (event == Event::GUI_TOGGLE_CONSOLE) {
			GUIManager::instance()->getConsole()->toggleShowHide();
		}
	}

	void Engine::makeScreenshot() {
		static int counter = 1;

		std::ostringstream fname;
		boost::filesystem::path file(boost::filesystem::initial_path());

		fname << "Screenshots/";

		file = boost::filesystem::system_complete( 
			boost::filesystem::path(fname.str().c_str(), boost::filesystem::native));

		// Check if Screenshots directory is present, and if not create it.
		if (!boost::filesystem::exists(file)) {
			boost::filesystem::create_directory(file);
		}

		fname << "screenshot" << counter << ".bmp";
		file = boost::filesystem::system_complete(
			boost::filesystem::path(fname.str().c_str(), boost::filesystem::native));

		while (boost::filesystem::exists(file)) {
			counter++;

			// Clear the stream.
			fname.str("");

			// We need to "re-insert" the counter integer that has been updated.
			fname << "Screenshots/" << "screenshot" << counter << ".bmp";

			// Then we must update the file var so that it is up-to-date with the fname var.
			file = boost::filesystem::system_complete( 
				boost::filesystem::path( fname.str().c_str(), boost::filesystem::native ) );
		}
		
		// The current backend makes the screenshot.
		RenderBackend* rback = RenderManager::instance()->current();
		if (rback) {
			rback->captureScreen(fname.str());
		}
	}


	void Engine::mainLoop() {
		// Get instance variables.
		GameStateManager* manager_gamestates = GameStateManager::instance();
		input::Manager* manager_input = input::Manager::instance();
		RenderBackend* rbackend = RenderManager::instance()->current();
		GUIManager* manager_gui = GUIManager::instance();
		TimeManager* manager_time = TimeManager::instance();

		// Register self as eventlistener and push the ENGINE_CONTEXT.
		manager_input->registerEventListener(input::Manager::ENGINE_CONTEXT, this);
		//manager_input->pushContext(InputManager::ENGINE_CONTEXT);

		// Main loop.
		while (m_run) {
			manager_time->update();

			rbackend->startFrame();

			manager_gamestates->turn();
			manager_input->handleEvents();
			manager_gui->turn();

			rbackend->endFrame();

			// Screenshots have to be made at this specific point in the 
			// loop, otherwise the screen might just be blank.
			if (m_makeScreenshot) {
				m_makeScreenshot = false;
				makeScreenshot();
			}

			ImageCache::instance()->collect();
			m_avgframetime = manager_time->getAverageFrameTime();
		}
		CScriptEngine()->callFunction("on_engine_stop");

		// Unregister self from the input manager.
		//manager_input->popContext(InputManager::ENGINE_CONTEXT);
		manager_input->unregisterListener(this);
	}

	double Engine::getAverageFrameTime( ) const {
		return m_avgframetime;
	}

	void Engine::printStatistics() const {
		std::cout
			<< "FIFE is " << (m_run ? "RUNNING" : "STOPPED") << "\n";
		TimeManager::instance()->printStatistics();
		ImageCache::instance()->printStatistics();
	}

}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
