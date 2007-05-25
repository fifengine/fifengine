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

#ifndef FIFE_EVENTCHANNEL_ICOMMANDIDS_H
#define FIFE_EVENTCHANNEL_ICOMMANDIDS_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE {
	/**  Types for different commands
	 */
	enum CommandType {
		CMD_UNKNOWN	 = -1,
		CMD_GO_NORTH = 1,
		CMD_GO_SOUTH = 2,
		CMD_GO_WEST  = 3,
		CMD_GO_EAST  = 4,
		// Unused, but may become useful.
		CMD_GO_NORTH_EAST = 5,
		CMD_GO_NORTH_WEST = 6,
		CMD_GO_SOUTH_EAST = 7,
		CMD_GO_SOUTH_WEST = 8,
		// Test toggles for the MapViewGameState.
		CMD_TEST_TOGGLE_OBJECTS   = 201,
		CMD_TEST_TOGGLE_FLOOR     = 202,
		CMD_TEST_TOGGLE_ROOF      = 203,
		CMD_TEST_TOGGLE_TILE_GRID = 204,
		CMD_TEST_TOGGLE_OBJ_GRID  = 205,
		CMD_TEST_CHANGE_ELEVATION = 206,
		CMD_TEST_INCREASE_ALPHA   = 207,
		CMD_TEST_DECREASE_ALPHA   = 208,
		CMD_TEST_TOGGLE_COORDS    = 209,
		CMD_TEST_TOGGLE_FADEOUT   = 210,
		// Used by Engine.
		CMD_MAKE_SCREENSHOT = 401,
		CMD_QUIT_GAME       = 402,
	};

} //FIFE

#endif
