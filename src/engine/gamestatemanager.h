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

#ifndef FIFE_GAMESTATEMANAGER_H
#define FIFE_GAMESTATEMANAGER_H

#include "singleton.h"
#include <map>
#include <vector>
#include <string>

namespace FIFE {
	class GameState;
	/** A simple state-machine (SM) for objects capsuling the game-specific code.
	 *
	 * A (derived) \c GameState contains the code to actually do something; any number of
	 * states may be active at any time. The engine will delegate to the \c turn function
	 * of all active states (in the order they were made active).
	 *
	 * @see GameState
	 *
	 * This class is derived from our \em singleton implementation; this means you can
	 * access the instance as \verbatim FIFE::GameStateManager::instance() \endverbatim as
	 * soon as the engine is running.
	 *
	 * @see DynamicSingleton
	 *
	 * \note The function activate does checks whether a state is already active;
	 * It will currently issue a warning and \b not activate a again.
	 *
	 * \bug Trying to deactivate a non-active state will currently only print log-msgs,
	 * throwing an exception might be more appropriate.
	 *
	 * Duplicate?
   *
   * GameStates get registered with this manager. Here they are stored
	 * updated, activated and deactivated. GameStates automatically register
	 * themselves. The turn() function gets called once a turn by
	 * Engine::mainLoop.
	 *
	 * @see GameState, Engine
   */
	class GameStateManager : public DynamicSingleton<GameStateManager> {
		public:
			/** Constructor. 
			 */
			GameStateManager();
			
			// 'virtual' destructor is required
			/** Destructor.
			 */
			virtual ~GameStateManager();

			/** Used by the GameState baseclass constructor; don't call from elsewhere for now.
			 * @param gamestate A pointer to a GameState instance
			 *
			 * The state will not be activated automatically, you have to do that!
			 *
			 * \bug replace PANIC_PRINT with exception
			 */
			void registerState(GameState*);
			
			/** Used by the GameState baseclass destructor; don't call from elsewhere for now.
			 * @param gamestate A pointer to a GameState instance
			 *
			 * \bug replace PANIC_PRINT with exception
			 */
			void unregisterState(GameState*);

			/** Query if a certain state is known (registed) to the SM.
			 *
			 * \param name The name of the state
			 * \return True if the state exists.
			 *
			 * \note This doesn't mean \b active; just that it exists.
			 */
			bool gameStateExists(const std::string& name) const;
			
			/** Access to the existing states by name.
			 * \param name The name of the state
			 * \return Pointer to requested state (if exists)
			 *
			 * \throws NotFound exception on failure.
			 */
			GameState* getGameState(const std::string& name) const;

			/** Activates a named GameState.
			 *
			 * Throws on failure. Calls the GameState::activate() function.
			 * 
			 * @param name The name of the GameState to be activated.
			 */
			void activate(const std::string& name);

			/** Deactivates a named GameState. 
			 *
			 * Throws on failure. Calls the GameState::deactivate() function.
			 * 
			 * @param name The name of the GameState to be deactivated.
			 */
			void deactivate(const std::string & name);

			/** Updates active GameStates.
			 * 
			 * Called by Engine::mainLoop() once every frame. It calls each
			 * active GameState's GameState::turn() function.
			 */
			void turn();

			/** Check whether a given state is active.
			 *
			 */
			bool isActive(const std::string& name) const;

		private:
			typedef std::map<std::string, GameState*> type_gamestates;

			/* FIXME: replace this with a std::map; or use a combination of gameStateExists() && getGameState()
			 * in (de)-activate to prevent multiple activation...
			 * No double Activation anymore. Hope this closes this Fixme --phoku
			 */
			typedef std::vector<GameState*> type_gamestates_active;

			// map containing all gamestates, active or inactive, and
			// their names.
			type_gamestates m_gamestates;

			// vector containing all active game states.
			type_gamestates_active m_gamestates_active;

	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
