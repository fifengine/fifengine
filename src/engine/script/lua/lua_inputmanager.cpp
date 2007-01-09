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

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "input/manager.h"
#include "exception.h"
#include "log.h"

#include "lua_inputmanager.h"

namespace FIFE {

	int InputManager_LuaScript::mapKeyToEventId(lua_State *L) {
		std::string context(luaL_checkstring(L, 1));
		int sdlkey = static_cast<int> (luaL_checknumber(L, 2));
		int eventid = static_cast<int> (luaL_checknumber(L, 3));
		Log("InputManager") << "binding " << sdlkey << " to " << eventid << " in context " << context;
		input::Manager::instance()->mapKeyToEventId(context,sdlkey,eventid);
		return 0;
	}

	int luaopen_inputmanager(lua_State *L) {
		luaL_openlib(L, "inputmanager", InputManager_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, InputManager_LuaScript::name}

	const luaL_reg InputManager_LuaScript::methods[] = {
		method(mapKeyToEventId),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
