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

// 3rd party library includes

// FIFE includes
#include "engine.h"
#include "events.h"

#include "lua_engine.h"

namespace FIFE {

	int Engine_LuaScript::makeScreenshot(lua_State *L) {
		// FIXME: Better way to access screenshot functionality
		Engine::instance()->handleEvent(Event::MAKE_SCREENSHOT);
		return 0;
	}

	int Engine_LuaScript::stop(lua_State *L) {
		Engine::instance()->stop();
		return 0;
	}

	int Engine_LuaScript::printStatistics(lua_State *L) {
		Engine::instance()->printStatistics();
		return 0;
	}

	int Engine_LuaScript::getFPS(lua_State *L) {
		lua_pushnumber(L, 1e3/Engine::instance()->getAverageFrameTime() );
		return 1;
	}

	int Engine_LuaScript::getCommandLine(lua_State *L) {
		lua_newtable(L);
		const std::vector<std::string>& cmdline = Engine::instance()->getCommandLine();
		for(size_t i=0; i!= cmdline.size(); ++i) {
			lua_pushstring(L, cmdline[i].c_str());
			lua_rawseti(L,-2,i+1);// index: 1..n in lua
		}
		return 1;
	}

	int luaopen_engine(lua_State *L) {
		luaL_openlib(L, "engine", Engine_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, Engine_LuaScript::name}

	const luaL_reg Engine_LuaScript::methods[] = {
		method(makeScreenshot),
		method(printStatistics),
		method(stop),
		method(getFPS),
		method(getCommandLine),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
