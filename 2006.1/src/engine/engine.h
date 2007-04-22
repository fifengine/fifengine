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

#ifndef FIFE_ENGINE_H
#define FIFE_ENGINE_H

#include <SDL.h>
#include "singleton.h"
#include "inputlistener.h"

namespace FIFE {

	/**  The engine kernel. 
     *
     * This object is created by the client app using Engine::instance().
	 * It's constructor and destructor create and destroy the various
	 * engine system managers. Once the gamestate and other settings have
	 * been propperly initialised by the client code the client calls
	 * start() to enter the main loop. The class gets registered as an input
	 * listener when it enters the mainLoop() function.
	 * \bug This should really be a dynamic singleton, too, and take argc,argv as Constructor arguments 
	 * ...and call Log::parseCmdLine()
	 *
     */
	class Engine : public StaticSingleton<Engine>, public InputListener {
		public:
			/** Enters the main loop.
			 * 
			 * @see mainLoop.
			 */
			void start();

			/** Stops the main loop before its next frame.
			 * 
			 * @see mainLoop.
			 */
			void stop();

			/** Get the average time between frames in milliseconds.
			 * 
			 * @see TimeManager::getAverageFrameTime()
			 */
			double getAverageFrameTime() const;

			/** Handles  event.
			 * 
			 * Specifically this handles the QUIT_GAME and MAKE_SCREENSHOT
			 * events. On a MAKE_SCREENSHOT event it sets a flag that will
			 * trigger a call to makeScreenshot() next frame in mainLoop().
			 * On a QUIT_GAME event it will call stop()
			 *
			 * @see stop(),makeScreenshot(),mainLoop()
			 */
			virtual void handleEvent(int event);

		private:
			// True if the mainLoop shuold keep running, false to stop it.
			bool m_run;
			// True, if in the next turn a screen shot shall be made
			bool m_makeScreenshot;
			// The average time between frames in milliseconds.
			double m_avgframetime;

			/** Initialised SDL, SDL_TTF, and the render backend. Called by constructor.
			 * \bug SLD_EnableKeyRepeat call is duplicated in backend!
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
			 * \note It does not push/pop the Engine Context, since game states may
			 * be activated before entering mainLoop.
			 *
			 * The Loop is ended if someone calls Engine::stop(), which will
			 * set a corresponding flag. 
			 */
			void mainLoop();

			/** Make a screenshot. Creates a proper filename and lets the backend do the rest.
			 */
			void makeScreenshot();

			/** Creates all the singletons used in the engine. Called by constructor.
			 */
			void createSingletons();

			/** Deletes all the singletons used in the engine. Called by destructor.
			 */
			void killSingletons();

			// Required to enforce singleton. Declares constructor/destructor and 
			// StaticSingleton as friend.
			SINGLEFRIEND(Engine);
	};

}//FIFE

#endif
