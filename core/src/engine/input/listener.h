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

#ifndef FIFE_INPUT_LISTENER_H
#define FIFE_INPUT_LISTENER_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL_events.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace input {
  /** Interface class for input listeners.
   *
   * Classes that wish to be informed specifically about key or mouse events
	 * inherit from this class and override the virtual functions that suit
	 * their needs. They then use input::Manager::registerKeyListener() or
	 * input::Manager::registerMouseButtonListener() to register themselves to
	 * be alerted of specific events.
	 * They can also use input::Manager::registerEventListener() to register
	 * to bound key events. This way, the key bindings are moved out of the code.
	 *
	 * @see input::Manager, Events
   */
	class Listener {
    public:
			virtual ~Listener() {}

			/** Called by InputManager to alert of bound keys.
       *
       * @param event The event to be handled.
			 */
			virtual void handleEvent(int event) {}

			/** Called by InputManager to alert of key event.
			 *
			 * @param event The keyboard event to be handled.
			 */
			virtual void handleKeyEvent(const SDL_KeyboardEvent& event) {}

			/** Called by InputManager to alert of mouse event.
			 *
			 * @param event The mouse event to be handled.
			 */
			virtual void handleMouseButtonEvent(const SDL_MouseButtonEvent& event) {}

		private:
	};

	/** Interface class for raw input listeners.
   *
   * A class that wishes to register itself at the InputManager's
	 * forced listener, which is alerted of all messages through the 
	 * handleMessage() function will inherit from this class, then call 
	 * InputManager::setForcedListener(). It will then be passed all 
	 * events.
	 *
	 * @see InputManager
   */
	class ListenerRaw {
		public:
			virtual ~ListenerRaw() {}

			/** Called by InputManager to alert of event.
			 *
			 * @param event The event to be handled.
			 */
			virtual void handleEvent(SDL_Event* event) = 0;
	};
} } // FIFE::input

#endif
