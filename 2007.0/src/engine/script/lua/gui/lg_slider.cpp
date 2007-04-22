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

	Slider::Slider(lua_State *L) : AWidget(), gcn::Slider() {
		switch(lua_gettop(L)) {
			case 0:
				break;
			case 1:
				setScaleEnd(double(luaL_checknumber(L, 1)));
				break;
			case 2:
				setScaleStart(double(luaL_checknumber(L, 1)));
				setScaleEnd(double(luaL_checknumber(L, 2)));
				break;
			default:
				break;
		}
	}
	Slider::~Slider() {}
	int Slider::l_getScale(lua_State *L) {
		lua_pushnumber(L, getScaleStart());
		lua_pushnumber(L, getScaleEnd());
		return 2;
	}
	int Slider::l_setScale(lua_State *L) {
		double s = double(luaL_checknumber(L, 1));
		double e = double(luaL_checknumber(L, 2));
		gcn::Slider::setScale(s, e);
		return 0;
	}
	int Slider::l_getValue(lua_State *L) {
		double v = getValue();
		lua_pushnumber(L, v);
		return 1;
	}
	int Slider::l_setValue(lua_State *L) {
		double v = double(luaL_checknumber(L, 1));
		setValue(v);
		return 0;
	}
	int Slider::l_getOrientation(lua_State *L) {
		unsigned int k = getOrientation();
		lua_pushnumber(L, k);
		return 1;
	}
	int Slider::l_setOrientation(lua_State *L) {
		unsigned int k = int(luaL_checknumber(L, 1));
		setOrientation(k);
		return 0;
	}


	const char Slider::className[] = "Slider";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Slider>::RegType Slider::methods[] = {
		method(Slider, getValue),
		method(Slider, setValue),
		method(Slider, setScale),
		method(Slider, getScale),
		method(Slider, getOrientation),
		method(Slider, setOrientation),

		LUAGUI_WIDGET_DEF(Slider),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
