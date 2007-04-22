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
	
	Button::Button(lua_State *L) : gcn::Button(luaL_checkstring(L,1)) {
		adjustSize();
	}
	Button::~Button() {}
	void mouseClick(int x, int y, int button, int count) {
		FIFE::Log("Button") << "CLICKED!";
	}
	int Button::l_isPressed(lua_State *L) {
		lua_pushboolean(L, gcn::Button::isPressed());
		return 1;
	}
	int Button::l_adjustSize(lua_State *L) {
		gcn::Button::adjustSize();
		return 0;
	}
	int Button::l_getAlignment(lua_State *L) {
		lua_pushnumber(L, gcn::Button::getAlignment());
		return 1;
	}
	int Button::l_setAlignment(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		gcn::Button::setAlignment(v);
		return 0;
	}
	int Button::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::Button::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int Button::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::Button::setCaption(v);
		adjustSize();
		return 0;
	}
	void Button::mouseIn() {
		FIFE::Log("Button") << "Mouse in";
	}

	const char Button::className[] = "Button";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Button>::RegType Button::methods[] = {
		method(Button, isPressed),
		method(Button, adjustSize),
		method(Button, getAlignment),
		method(Button, setAlignment),
		method(Button, getCaption),
		method(Button, setCaption),
		method(Button, setFont),

		LUAGUI_WIDGET_DEF(Button),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
