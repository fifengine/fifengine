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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "lua_mapcamera.h"
#include "lua_mapcontrol.h"

namespace FIFE {

	MapCamera_Lunar::MapCamera_Lunar(lua_State *L) 
		: map::Camera(Lunar<MapControl_Lunar>::check(L,1)) {
	}
	MapCamera_Lunar::~MapCamera_Lunar() {}

	int MapCamera_Lunar::l_render(lua_State *L) {
		map::Camera::render();
		return 0;
	}

	int MapCamera_Lunar::l_setViewport(lua_State *L) {
		map::Camera::setViewport(Rect(luaL_checkinteger(L,1),
		                              luaL_checkinteger(L,2),
		                              luaL_checkinteger(L,3),
		                              luaL_checkinteger(L,4)));
		return 0;
	}

	int MapCamera_Lunar::l_moveBy(lua_State *L) {
		map::Camera::moveBy(Point(luaL_checkinteger(L,1),luaL_checkinteger(L,2)));
		return 0;
	}

	int MapCamera_Lunar::l_jumpTo(lua_State *L) {
		map::Camera::jumpTo(Point(luaL_checkinteger(L,1),luaL_checkinteger(L,2)));
		return 0;
	}

	int MapCamera_Lunar::l_moveTo(lua_State *L) {
		map::Camera::moveTo(Point(luaL_checkinteger(L,1),luaL_checkinteger(L,2)));
		return 0;
	}

	int MapCamera_Lunar::l_track(lua_State *L) {
		map::Camera::track(luaL_checkinteger(L,1));
		return 0;
	}


	const char* MapCamera_Lunar::className = "MapCamera";

#define method(name) {#name, &MapCamera_Lunar::l_ ## name}

	Lunar<MapCamera_Lunar>::RegType MapCamera_Lunar::methods[] = {
		method(setViewport),
		method(render),
		method(moveBy),
		method(jumpTo),
		method(moveTo),
		method(track),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
