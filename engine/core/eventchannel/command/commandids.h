/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_EVENTCHANNEL_ICOMMANDIDS_H
#define FIFE_EVENTCHANNEL_ICOMMANDIDS_H

// Standard C++ library includes
//

// 3rd party library includes
//
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE {
	/**  Types for different commands
	 */
	enum CommandType {
		CMD_UNKNOWN = -1,
		CMD_QUIT_GAME = SDL_WINDOWEVENT_CLOSE,
		CMD_MOUSE_FOCUS_GAINED = SDL_WINDOWEVENT_ENTER,
		CMD_MOUSE_FOCUS_LOST = SDL_WINDOWEVENT_LEAVE,
		CMD_INPUT_FOCUS_GAINED = SDL_WINDOWEVENT_FOCUS_GAINED,
		CMD_INPUT_FOCUS_LOST = SDL_WINDOWEVENT_FOCUS_LOST,
		CMD_APP_RESTORED = SDL_WINDOWEVENT_SHOWN,
		CMD_APP_ICONIFIED = SDL_WINDOWEVENT_HIDDEN,
	};

} //FIFE

#endif
