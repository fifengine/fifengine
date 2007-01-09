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

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "guichan_addon/console.h"
#include "video/gui/guimanager.h"

#include "lua_console.h"

namespace FIFE {

	static Console* getConsole(lua_State* L) {
		Console* console = GUIManager::instance()->getConsole();
		if( console == 0 ) {
			lua_pushstring(L,"Console not available yet.");
			lua_error(L);
		}
		return console;
	}

	int Console_LuaScript::print(lua_State *L) {
		const char* cmessage;

		while( lua_gettop(L) != 0 ) {
			lua_pushvalue(L,1);
			lua_remove(L,1);

			lua_getglobal(L, "tostring");
			lua_insert(L,lua_gettop(L) - 1);
	
			if( 0 != lua_pcall(L,1,1,0) ) { 
				// Error, top is error message
				cmessage = lua_tostring(L,-1);
			} else {
				// Ok, top is retval
				cmessage = lua_tostring(L,-1);
			}
			if( cmessage ) {
				getConsole(L)->println(cmessage);
			}
			lua_pop(L,1);
		}

		return 0;
	}

	int Console_LuaScript::clear(lua_State *L) {
		getConsole(L)->clear();
		return 0;
	}
	int Console_LuaScript::show(lua_State *L) {
		getConsole(L)->show();
		return 0;
	}
	int Console_LuaScript::hide(lua_State *L) {
		getConsole(L)->hide();
		return 0;
	}

	int luaopen_console(lua_State *L) {
		luaL_openlib(L, "console", Console_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, Console_LuaScript::name}

	const luaL_reg Console_LuaScript::methods[] = {
		method(show),
		method(hide),
		method(print),
		method(clear),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
