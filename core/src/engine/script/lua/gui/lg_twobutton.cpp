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
#include "util/log.h"

#include "luagui.h"

namespace luaGui {

	TwoButton::TwoButton(lua_State *L) : gcn::TwoButton(
		static_cast<userdataType*>(luaL_checkudata(L, 1, luaGui::Image::className))->pT,
		static_cast<userdataType*>(luaL_checkudata(L, 2, luaGui::Image::className))->pT) {
		int t = lua_gettop(L);
		if (t == 3) {
			const char* caption = luaL_checkstring(L, 3);
			setCaption(caption);
		}
	}
	int TwoButton::l_getAlignment(lua_State *L) {
		lua_pushnumber(L, gcn::Button::getAlignment());
		return 1;
	}
	int TwoButton::l_setAlignment(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		gcn::Button::setAlignment(v);
		return 0;
	}
	int TwoButton::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::Button::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int TwoButton::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::Button::setCaption(v);
		return 0;
	}
	const char TwoButton::className[] = "TwoButton";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<TwoButton>::RegType TwoButton::methods[] = {
		method(TwoButton, getAlignment),
		method(TwoButton, setAlignment),
		method(TwoButton, getCaption),
		method(TwoButton, setCaption),
		method(TwoButton, setFont),
		LUAGUI_WIDGET_DEF(TwoButton),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
