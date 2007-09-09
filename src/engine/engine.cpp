/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "graphix_primitives.h"
#include "vfs.h"
#include "vfshostsystem.h"
#include "settingsmanager.h"
#include "scriptbase.h"

#include <SDL.h>
#include <SDL_ttf.h>

namespace FIFE {

	Engine::Engine() : m_screen(0), m_run(false), m_global_gamma(1.0), m_gui(0) {
		initSDL();
	}

	Engine::~Engine() {
		quit();
	}

	Engine::Engine(const Engine&) : m_screen(0), m_run(false), m_global_gamma(1.0), m_gui(0) {}

	Engine& Engine::operator=(const Engine&) {
		return *this;
	}

	void Engine::quit() {
		VFS::instance()->cleanup();
		SettingsManager::instance()->saveSettings();

		ScriptEngine *se = ScriptEngine::instance();
		se->stop();
		delete se;

		delete m_gui;

		TTF_Quit();
		SDL_Quit();
	}

	GUI* Engine::getGUI() const {
		return m_gui;
	}

	void Engine::initSDL() {
		Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE;
		if (SDL_Init(flags) < 0) throw SDLException(SDL_GetError());
		SDL_EnableUNICODE(1);
		TTF_Init();
	}

	SDL_Surface* Engine::createMainScreen(int width, int height, int bpp, bool fullscreen, bool hwsurface, bool use_gl, bool use_db) {
		m_screen = 0;

		prepareDisplay(use_gl, use_db);
		Uint32 flags = getFlags();
		hwsurface ?  flags |= SDL_HWSURFACE : flags |= SDL_SWSURFACE;
		if (fullscreen)
			flags |= SDL_FULLSCREEN;

		if (!SDL_VideoModeOK(width, height, bpp, flags))
			throw SDLException("Videomode not available");

		m_screen = SDL_SetVideoMode(width, height, bpp, flags);
		initGL(width, height);

		/* Set title for window */
		SDL_WM_SetCaption("FIFE", NULL);

		if (!m_screen)
			throw SDLException(SDL_GetError());

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
		setGlobalGamma(1.0);

		SDL_EnableUNICODE(1);

		//Console::instance()->init(m_screen);
		VFS::instance()->addSource(new VFSHostSystem());

		m_gui = new GUI(use_gl,m_screen);

		return m_screen;
	}

	void Engine::start() {
		if (!m_screen) return;

		m_run = true;
		mainLoop();
	}

	void Engine::stop() {
		m_run = false;
	}

	void Engine::handleKeyEvent(const SDL_KeyboardEvent& event) {
		if (event.keysym.sym == SDLK_ESCAPE && event.state == SDL_RELEASED) {
			stop();
		}
	}

	void Engine::mainLoop() {
		GameStateManager* manager_gamestates = GameStateManager::instance();
		InputManager* manager_input = InputManager::instance();

		manager_input->registerKeyListener(SDLK_ESCAPE, this);
		//Console* console = Console::instance();
// 		ScriptEngine* se = ScriptEngine::instance();
		//console->write("FIFE console - powered by:");
		//console->write(se->implementationName.c_str());

		while (m_run) {
			clearScreen(m_screen);

			manager_gamestates->turn();
			manager_input->handleEvents();
			//console->draw();
			m_gui->drawGUI();

			swapBuffers(m_screen);
		}

		manager_input->unregisterKeyListener(this);
	}

	void Engine::setGlobalGamma(float g) {
		m_global_gamma = g;
		SDL_SetGamma(m_global_gamma, m_global_gamma, m_global_gamma);
	}

	float Engine::getGlobalGamma() const {
		return m_global_gamma;
	}
	int Engine::screenWidth() const {
		if( m_screen )
			return m_screen->w;
		return 0;
	};
	int Engine::screenHeight() const {
		if( m_screen )
			return m_screen->h;
		return 0;
	};


}//FIFE

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
