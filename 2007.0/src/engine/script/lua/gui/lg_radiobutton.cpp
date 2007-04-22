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
#include "log.h"

#include "luagui.h"

namespace luaGui {

	RadioButton::RadioButton(lua_State *L) : AWidget(), gcn::RadioButton() {
		switch(lua_gettop(L)) {
			case 0:
				break;
			case 2:
				setCaption(luaL_checkstring(L, 1));
				setGroup(luaL_checkstring(L, 2));
				setMarked(false);
				break;
			case 3:
				setCaption(luaL_checkstring(L, 1));
				setGroup(luaL_checkstring(L, 2));
				setMarked(lua_toboolean(L, 3));
				break;
			default:
				break;
		}
		adjustSize();
	}
	RadioButton::~RadioButton() {}
	int RadioButton::l_isMarked(lua_State *L) {
		lua_pushboolean(L, gcn::RadioButton::isMarked());
		return 1;
	}
	int RadioButton::l_setMarked(lua_State *L) {
		bool v = bool(lua_isboolean(L, 1));
		gcn::RadioButton::setMarked(v);
		return 0;
	}
	int RadioButton::l_getCaption(lua_State *L) {
		const std::string & sr = getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int RadioButton::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		setCaption(v);
		adjustSize();
		return 0;
	}

	const char RadioButton::className[] = "RadioButton";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<RadioButton>::RegType RadioButton::methods[] = {
		method(RadioButton, getCaption),
		method(RadioButton, setCaption),
		method(RadioButton, isMarked),
		method(RadioButton, setMarked),
		method(RadioButton, setFont),

		LUAGUI_WIDGET_DEF(RadioButton),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
