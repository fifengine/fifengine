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

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "log.h"

#include "lua_map.h"
#include "lua_mapcontrol.h"

namespace FIFE {

	MapControl_Lunar::MapControl_Lunar(lua_State *L) {
	}

	MapControl_Lunar::~MapControl_Lunar() {
	}

	int MapControl_Lunar::l_start(lua_State *L) {
		map::Control::start();
		return 0;
	}

	int MapControl_Lunar::l_stop(lua_State *L) {
		map::Control::stop();
		return 0;
	}

	int MapControl_Lunar::l_turn(lua_State *L) {
		map::Control::turn();
		return 0;
	}

	int MapControl_Lunar::l_load(lua_State *L) {
		const char* cfilename = luaL_checkstring(L,1);
		if( cfilename ) {
			map::Control::load( cfilename );
		}
		return 0;
	}

	int MapControl_Lunar::l_setMap(lua_State *L) {
		map::MapPtr map = Lunar<Map_LuaScript>::check(L,1)->getMap();
		map::Control::setMap(map);
		return 0;
	}

	int MapControl_Lunar::l_clearMap(lua_State *L) {
		map::Control::clearMap();
		return 0;
	}

	int MapControl_Lunar::l_getMap(lua_State *L) {
		map::MapPtr map = map::Control::getMap();
		if( !map ) {
			lua_pushnil(L);
		} else {
			Lunar<Map_LuaScript>::push(L,new Map_LuaScript(map),true);
		}
		return 1;
	}

	int MapControl_Lunar::l_isRunning(lua_State *L) {
		lua_pushboolean(L,map::Control::isRunning());
		return 1;
	}

	int MapControl_Lunar::l_activateElevation(lua_State *L) {
		int elevation = luaL_checkinteger(L,1);
		map::Control::activateElevation( elevation );
		return 0;
	}


	const char* MapControl_Lunar::className = "MapControl";

#define method(class, name) {#name, &class::l_  ## name}

	Lunar<MapControl_Lunar>::RegType MapControl_Lunar::methods[] = {
		method(MapControl_Lunar, start),
		method(MapControl_Lunar, stop),
		method(MapControl_Lunar, turn),
		method(MapControl_Lunar, load),
		method(MapControl_Lunar, setMap),
		method(MapControl_Lunar, getMap),
		method(MapControl_Lunar, clearMap),
		method(MapControl_Lunar, isRunning),
		method(MapControl_Lunar, activateElevation),
		{0,0}
	};

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
