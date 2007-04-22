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
#include "audio/manager.h"
#include "exception.h"
#include "log.h"

#include "lua_audiomanager.h"

namespace FIFE {

	int AudioManager_LuaScript::setVolume(lua_State *L) {
		float tmpvol = float(luaL_checknumber(L, 1));
		audio::Manager::instance()->setVolume(tmpvol);
		return 0;
	}

	/*
	int AudioManager_LuaScript::mute(lua_State *L) {
		audio::Manager::instance()->mute();
		return 0;
	}

	int AudioManager_LuaScript::unmute(lua_State *L) {
		audio::Manager::instance()->unmute();
		return 0;
	}*/

	int AudioManager_LuaScript::setAmbientSound(lua_State *L) {
		const char* as_char_p = lua_tostring(L, 1);
		if (as_char_p == NULL) {
			// should Log() something
			return 0;
		}
		const std::string as_string(as_char_p);
		try {
			audio::Manager::instance()->setAmbientSound(as_string);
		}
		catch (NotFound & e) {
			Log("audioman_lua") << "Caught exception on loading music: " << e.getMessage();
		}
		return 0;
	}

	int luaopen_audiomanager(lua_State *L) {
		luaL_openlib(L, "audiomanager", AudioManager_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, AudioManager_LuaScript::name}

	const luaL_reg AudioManager_LuaScript::methods[] = {
		method(setVolume),/*
		method(mute),
		method(unmute),*/
		method(setAmbientSound),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
