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
#include "debugutils.h"

#include "luagui.h"

namespace luaGui {

	Color::Color(lua_State *L) : gcn::Color() {
		int num_var = lua_gettop(L);
		switch(num_var) {
			case 0:
				break;
			case 3:
				r = int(luaL_checknumber(L, 1));
				g = int(luaL_checknumber(L, 2));
				b = int(luaL_checknumber(L, 3));
				break;
			case 4:
				r = int(luaL_checknumber(L, 1));
				g = int(luaL_checknumber(L, 2));
				b = int(luaL_checknumber(L, 3));
				a = int(luaL_checknumber(L, 4));
				break;
			default:
				FIFE::Log("luaGui::Color", FIFE::Log::LEVEL_WARN) << "Invalid number of arguments: " << num_var;
		}
	}
	int Color::l_getRed(lua_State *L) {
		lua_pushnumber(L, double(r));
		return 1;
	}
	int Color::l_getGreen(lua_State *L) {
		lua_pushnumber(L, double(g));
		return 1;
	}
	int Color::l_getBlue(lua_State *L) {
		lua_pushnumber(L, double(b));
		return 1;
	}
	int Color::l_getAlpha(lua_State *L) {
		lua_pushnumber(L, double(a));
		return 1;
	}

	int Color::l_setRed(lua_State *L) {
		r = int(luaL_checknumber(L, 1));
		return 0;
	}
	int Color::l_setGreen(lua_State *L) {
		g = int(luaL_checknumber(L, 1));
		return 0;
	}
	int Color::l_setBlue(lua_State *L) {
		b = int(luaL_checknumber(L, 1));
		return 0;
	}
	int Color::l_setAlpha(lua_State *L) {
		a = int(luaL_checknumber(L, 1));
		return 0;
	}

	const char Color::className[] = "Color";
#define method(name) {#name, &Color::l_ ## name}

	Lunar<Color>::RegType Color::methods[] = {
		method(setRed),
		method(setGreen),
		method(setBlue),
		method(setAlpha),
		method(getRed),
		method(getGreen),
		method(getBlue),
		method(getAlpha),
		{0,0}
	};

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
