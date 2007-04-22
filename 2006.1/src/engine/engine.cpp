/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "engine.h"
#include "exception.h"
#include "gamestatemanager.h"
#include "gamestate.h"
#include "inputmanager.h"
#include "debugutils.h"
#include "vfs.h"
#include "vfshostsystem.h"
#include "settingsmanager.h"
#include "scriptbase.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"

#include "vfs.h"
#include "vfssourcefactory.h"
#include "settingsmanager.h"
#include "scriptbase.h"
#include "rendermanager.h"
#include "gamestatemanager.h"
#include "inputmanager.h"
#include "imagecache.h"
#include "mapfactory.h"
#include "audiomanager.h"
#include "gui/guimanager.h"
#include "guichan_addon/console.h"
#include "timemanager.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "events.h"

namespace FIFE {

	Engine::Engine() :m_run(false),m_makeScreenshot(false), m_avgframetime(0) {
		createSingletons();
		init();
	}

	Engine::~Engine() {
		killSingletons();
		TTF_Quit();
		SDL_Quit();
	}

	void Engine::createSingletons() {
		new SettingsManager();
		new GUIManager();
		new RenderManager();
		new VFSSourceFactory();
		new VFS();

		VFS::instance()->addSource(new VFSHostSystem());

		new InputManager();
		new ImageCache();
		new MapFactory();
		new AudioManager();
		new GameStateManager();
		//new ScriptEngine();
		new TimeManager();
	}

	void Engine::killSingletons() {
		delete ScriptEngine::instance();
		delete GameStateManager::instance();
		delete AudioManager::instance();
		delete MapFactory::instance();
		delete ImageCache::instance();
		delete VFS::instance();
		delete VFSSourceFactory::instance();
		delete RenderManager::instance();
		delete GUIManager::instance();
		delete InputManager::instance();
		delete SettingsManager::instance();
		delete TimeManager::instance();
	}

	void Engine::init() {
		if(SDL_Init(SDL_INIT_NOPARACHUTE | SDL_INIT_TIMER) < 0) {	//if failed to init SDL throw exception.
			throw SDLException(SDL_GetError());
		}

		// this may be a good idea; the guichan examples do it, I don't know why...
		SDL_EnableUNICODE(1);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

		TTF_Init();

		// Select the render backend.
		RenderManager::instance()->selectBackend(SettingsManager::instance()->read<std::string>("RenderBackend", "SDL"));
	}

	void Engine::start() {
		ScriptEngine::instance()->callFunction("on_engine_start");
		m_run = true;
		mainLoop();
	}

	void Engine::stop() {
		m_run = false;
	}

	void Engine::handleEvent(int event) {
		if ( event == Event::QUIT_GAME ) {
			stop();
		} else if ( event == Event::MAKE_SCREENSHOT) {
			m_makeScreenshot = true;
		} else if ( event == Event::GUI_TOGGLE_CONSOLE ) {
			GUIManager::instance()->getConsole()->toggleShowHide();
		}
	}

	void Engine::makeScreenshot() {
		static int counter = 1;

		std::ostringstream fname;
		boost::filesystem::path file( boost::filesystem::initial_path() );

		fname << "Screenshots/";

		file = boost::filesystem::system_complete( 
			boost::filesystem::path( fname.str().c_str(), boost::filesystem::native ) );

		// Check if Screenshots directory is present, and if not create it
		if (!boost::filesystem::exists( file )) {
			boost::filesystem::create_directory( file );
		}

		fname << "screenshot" << counter << ".bmp";
		file = boost::filesystem::system_complete(
			boost::filesystem::path( fname.str().c_str(), boost::filesystem::native ) );

		while (boost::filesystem::exists( file )) {
			counter++;

			// Clear the stream
			fname.str("");

			// We need to "re-insert" the counter integer that has been updated
			fname << "Screenshots/" << "screenshot" << counter << ".bmp";

			// Then we must update the file var so that it is up-to-date with the fname var
			file = boost::filesystem::system_complete( 
				boost::filesystem::path( fname.str().c_str(), boost::filesystem::native ) );
		}
		RenderBackend* rback = RenderManager::instance()->getCurrentBackend();
		if( rback ) {
			rback->captureScreen(fname.str());
		}
	}
		

	void Engine::mainLoop() {
		// Get instance variables.
		GameStateManager* manager_gamestates = GameStateManager::instance();
		InputManager* manager_input = InputManager::instance();
		RenderBackend* rbackend = RenderManager::instance()->getCurrentBackend();
		GUIManager* manager_gui = GUIManager::instance();
		TimeManager* manager_time = TimeManager::instance();

		// Register self as eventlistener and push the ENGINE_CONTEXT
		manager_input->registerEventListener(InputManager::ENGINE_CONTEXT, this);
		//manager_input->pushContext(InputManager::ENGINE_CONTEXT);

		while (m_run) {		//main loop.
			manager_time->update();

			rbackend->startFrame();

			manager_gamestates->turn();
			manager_input->handleEvents();
			manager_gui->turn();

			rbackend->endFrame();

			// Screenshots have to be made at this specific point in the 
			// loop, otherwise the screen might just be blank.
			if( m_makeScreenshot ) {
				m_makeScreenshot = false;
				makeScreenshot();
			}

			ImageCache::instance()->collect();
			m_avgframetime = manager_time->getAverageFrameTime();
		}

		// Unregister self from the input manager.
		//manager_input->popContext(InputManager::ENGINE_CONTEXT);
		manager_input->unregisterListener(this);
	}

	double Engine::getAverageFrameTime( ) const {
		return m_avgframetime;
	}

}

//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
