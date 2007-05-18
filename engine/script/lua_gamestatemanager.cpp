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
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/gamestate/gamestatemanager.h"
#include "util/log.h"
#include "util/exception.h"

#include "lua_gamestatemanager.h"

namespace FIFE {

	int GameStateManager_LuaScript::gameStateExists(lua_State *L) {
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException(std::string("String value expected:") + __FUNCTION__);

		const char * name = lua_tostring(L, -1);
		GameStateManager * gsm = FIFE::GameStateManager::instance();
		assert(gsm);
		lua_pushboolean(L, gsm->gameStateExists(name));
		return 1;
	}

	int GameStateManager_LuaScript::activate(lua_State *L) {
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException(std::string("String value expected:") + __FUNCTION__);

		const char * name = lua_tostring(L, -1);
		GameStateManager * gsm = FIFE::GameStateManager::instance();
		assert(gsm);
		gsm->activate(name);
		return 0;
	}
	int GameStateManager_LuaScript::deactivate(lua_State *L) {
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException(std::string("String value expected:") + __FUNCTION__);

		const char * name = lua_tostring(L, -1);
		GameStateManager * gsm = FIFE::GameStateManager::instance();
		assert(gsm);
		gsm->deactivate(name);
		return 0;
	}

	int GameStateManager_LuaScript::isActive(lua_State *L) {
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException(std::string("String value expected:") + __FUNCTION__);

		const char * name = lua_tostring(L, -1);
		GameStateManager * gsm = FIFE::GameStateManager::instance();
		assert(gsm);
		lua_pushboolean(L, gsm->isActive(name));
		return 1;
	}
		
		

	int luaopen_gsmanager(lua_State *L) {
		luaL_openlib(L, "state_manager", GameStateManager_LuaScript::methods, 0);
		return 0;
	}

#define method(name,rname) {#name, GameStateManager_LuaScript::rname}
	const luaL_reg GameStateManager_LuaScript::methods[] = {
		method(exists, gameStateExists),
		method(activate, activate),
		method(deactivate, deactivate),
		method(isActive,isActive),
		{NULL, NULL}
	};

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
