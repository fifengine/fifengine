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

#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

// Standard C++ library includes
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "input/listener.h"

#include "singleton.h"

namespace FIFE {

	/** This is the engine kernel.
	 *
	 * This object is created by the client app using Engine::instance().
	 * Its constructor and destructor create and destroy the various
	 * engine system managers. Once the gamestate and other settings have
	 * been properly initialised by the client code the client calls
	 * start() to enter the main loop. The class gets registered as an input
	 * listener when it enters the mainLoop() function.
	 *
	 */
	class Engine : public DynamicSingleton<Engine>, public input::Listener {
		public:
			/** Constructor
			 */
			Engine(int argc, char** argv);
			/** Destructor
			 */
			virtual ~Engine();


			/** Enters the main loop.
			 *
			 * @see mainLoop()
			 */
			void start();

			/** Stops the main loop before its next frame.
			 *
			 * @see mainLoop()
			 */
			void stop();

			/** Get the average time between frames in milliseconds.
			 *
			 * @return Average time between frames in milliseconds.
			 * @see TimeManager::getAverageFrameTime()
			 */
			double getAverageFrameTime() const;

			/** Print Engine Statistics
			 *  Prints TimeManager and ImageCache statistics
			 */
			void printStatistics() const;

			/** Handles event.
			 *
			 * Specifically, this handles the QUIT_GAME, MAKE_SCREENSHOT and
			 * GUI_TOGGLE_CONSOLE events.
			 * On a MAKE_SCREENSHOT event it sets a flag that will
			 * trigger a call to makeScreenshot() next frame in mainLoop().
			 * On a QUIT_GAME event it will call stop().
			 * On a GUI_TOGGLE_CONSOLE event it will call Console::toggleShowHide().
			 *
			 * @param event Event to be handled (only QUIT_GAME, MAKE_SCREENSHOT
			 * or GUI_TOGGLE_CONSOLE events are handled).
			 * @see stop(), makeScreenshot(), mainLoop(), Console::toggleShowHide()
			 */
			 virtual void handleEvent(int event);

		private:
			// True if the mainLoop should keep running, false to stop it.
			bool m_run;
			// True if in the next turn a screenshot shall be made.
			bool m_makeScreenshot;
			// The average time between frames in milliseconds.
			double m_avgframetime;
			/// Command line
			std::vector<std::string> m_cmdline;

			/** Initialise Engine. Called by constructor.
			 *
			 * Initialises SDL, SDL_ttf, the render backend,
			 * and creates the singletons.
			 *
			 * @bug SLD_EnableKeyRepeat call is duplicated in backend!
			 */
			void init();

			/** The main game loop. Updates/renders gamestate/gui.
			 * This is at the core of the engine functionality.
			 *
			 * It calls the following functions each turn:
			 *
			 * TimeManager::update()
			 * RenderBackend::startFrame()
			 * GameStateManager::turn()
			 * InputManager::handleEvents()
			 * GuiManager::turn()
			 * RenderBackend::endFrame()
			 * Possibly makeScreenshot()
			 * ImageCache::collect()
			 *
			 * At the beginning it registers itself with the InputManager as
			 * Listener to ENGINE_CONTEXT events.
			 * At the end it unregisters itself from Input Events.
			 * @note It does not push/pop the Engine Context, since game states may
			 * be activated before entering mainLoop.
			 *
			 * The Loop is ended if someone calls Engine::stop(), which will
			 * set a corresponding flag. 
			 */
			void mainLoop();

			/** Makes a screenshot. Creates a proper filename and lets the backend do the rest.
			 */
			void makeScreenshot();

			/** Deletes all the singletons used in the engine. Called by destructor.
			 */
			void killSingletons();
	};

}//FIFE

#endif
