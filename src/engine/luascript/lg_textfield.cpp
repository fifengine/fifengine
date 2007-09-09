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
	TextField::TextField(lua_State *L) : gcn::TextField(luaL_checkstring(L,1)) {
		adjustSize();
	}
	TextField::~TextField() {}
	int TextField::l_adjustSize(lua_State *L) {
		gcn::TextField::adjustSize();
		return 0;
	}
	int TextField::l_adjustHeight(lua_State *L) {
		gcn::TextField::adjustHeight();
		return 0;
	}
	int TextField::l_getCaretPosition(lua_State *L) {
		lua_pushnumber(L, gcn::TextField::getCaretPosition());
		return 1;
	}
	int TextField::l_setCaretPosition(lua_State *L) {
		unsigned int v = static_cast<unsigned int>(luaL_checknumber(L, 1));
		gcn::TextField::setCaretPosition(v);
		return 0;
	}
	int TextField::l_getText(lua_State *L) {
		const std::string & sr = gcn::TextField::getText();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int TextField::l_setText(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::TextField::setText(v);
		//adjustSize();
		return 0;
	}

	const char TextField::className[] = "TextField";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<TextField>::RegType TextField::methods[] = {
		method(TextField, adjustSize),
		method(TextField, getCaretPosition),
		method(TextField, setCaretPosition),
		method(TextField, getText),
		method(TextField, setText),
		method(TextField, setFont),

		LUAGUI_WIDGET_DEF(TextField),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
