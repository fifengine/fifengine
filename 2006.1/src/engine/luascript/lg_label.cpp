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

#include "log.h"
#include "luagui.h"
namespace luaGui {
	Label::Label(lua_State *L) : AWidget(), gcn::Label(luaL_checkstring(L,1)) {
		adjustSize();
	}
	Label::~Label() {}
	int Label::l_adjustSize(lua_State *L) {
		gcn::Label::adjustSize();
		return 0;
	}
	int Label::l_getAlignment(lua_State *L) {
		lua_pushnumber(L, gcn::Label::getAlignment());
		return 1;
	}
	int Label::l_setAlignment(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		gcn::Label::setAlignment(v);
		return 0;
	}
	int Label::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::Label::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int Label::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::Label::setCaption(v);
		adjustSize();
		return 0;
	}

	const char Label::className[] = "Label";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Label>::RegType Label::methods[] = {
		method(Label, adjustSize),
		method(Label, getAlignment),
		method(Label, setAlignment),
		method(Label, getCaption),
		method(Label, setCaption),
		method(Label, setFont),

		LUAGUI_WIDGET_DEF(Label),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
