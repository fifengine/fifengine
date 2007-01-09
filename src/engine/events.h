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

#ifndef FIFE_EVENTS_H
#define FIFE_EVENTS_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** FIFE Events.
	 * This class holds the Event IDs that InputListeners can register to
	 * with the InputManager.
	 *
	 * @warning Please pay attention, that this is kept in sync with bindings.lua.
	 * @see InputManager, InputListener
	 */
	class Event {
		public:
			enum {
				// Sentinel value, not used at all. 
				UNKNOWN_EVENT = 0,
				// Values used by the MapViewGameState.
				GO_NORTH = 1,
				GO_SOUTH = 2,
				GO_WEST  = 3,
				GO_EAST  = 4,
				// Unused, but may become useful.
				GO_NORTH_EAST = 5,
				GO_NORTH_WEST = 6,
				GO_SOUTH_EAST = 7,
				GO_SOUTH_WEST = 8,
				// Test toggles for the MapViewGameState.
				TEST_TOGGLE_OBJECTS   = 9,
				TEST_TOGGLE_FLOOR     = 10,
				TEST_TOGGLE_ROOF      = 11,
				TEST_TOGGLE_TILE_GRID = 12,
				TEST_TOGGLE_OBJ_GRID  = 13,
				TEST_CHANGE_ELEVATION = 14,
				TEST_INCREASE_ALPHA   = 15,
				TEST_DECREASE_ALPHA   = 16,
				// Used by Engine.
				MAKE_SCREENSHOT = 17,
				QUIT_GAME       = 18,
				// These are just examples.
				QUIT_MAP = 19,
				GUI_TOGGLE_CONSOLE = 20,
				// Another sentinel. Keep the value correct, although it is not used atm.
				NR_OF_EVENTS = 21
			};

		// Not Used. Are they NEEDED?		
		/*
		static const std::string& nameOfEventId(int eventid);
		static int eventIdOfName(const std::string& name);
		*/
		
	};

};//FIFE

#endif
