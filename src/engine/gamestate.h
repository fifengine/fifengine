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

#ifndef FIFE_GAMESTATE_H
#define FIFE_GAMESTATE_H

#include <string>

namespace FIFE {

	/** Abstract baseclass for main state-machine.
	 *
	 * Derived classes should implement the virtual functions \c activate,
	 * \c deactivate and \c turn; the base class automatically registers
	 * itself to the GameStateManager using the given name.
	 *
	 * @see GameStateManager for a more detailed description of the SM.
   *
	 * Duplicate?
	 *
   * Game states are registered with the game state manager automatically by
	 * their constructors, and unregistered by the destructor. When they are active
	 * their turn() function gets called once a frame. To create the main game state
	 * the client derives a class from this, passes an appropriete name to the
	 * GameState constructor and when the engine is ready, and before calling
	 * Engine::start(), creates an instance of this class, and then activates it
	 * using the GameStateManager::activate() function.
	 *
	 * @see GameStateManager
   */
	class GameState {
		public:
			/** Constructor. Registers self with GameStateManager.
			 * 
			 * @param name The unique name which the state will be registered as.
			 */
			GameState(const std::string& name);

			/** Destructor. Unregisters self with the GameStateManager.
			 */
			virtual ~GameState();

			/** Get the name by which the state was registered.
			 * 
			 * @return A const reference to the states registration name.
			 */
			const std::string& getName() const;

			/** Called (once) each time a gamestate is activated by GameStateManager::activate().
			 *
			 * @note Implement this in derived classes!
			 */
			virtual void activate(); // called when gamestate gets activated
			
			/** Called (once) each time a gamestate is deactivated by GameStateManager::deactivate().
			 *
			 * @note Implement this in derived classes!
			 */
			virtual void deactivate(); // called when gamestate gets deactivated
			
			/** Called (continuously) by GameStateManager::turn() while a gamestate is active.
			 *
			 * @note Implement this in derived classes!
			 */
			virtual void turn(); // called while gamestate is active

		private:
			// The name which the state is registered as.
			std::string m_name;
	};

}//FIFE

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
