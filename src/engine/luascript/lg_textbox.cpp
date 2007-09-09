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
	TextBox::TextBox(lua_State *L) : gcn::TextBox(luaL_checkstring(L,1)) {
	}
	TextBox::~TextBox() {}
	int TextBox::l_getCaretPosition(lua_State *L) {
		lua_pushnumber(L, gcn::TextBox::getCaretPosition());
		return 1;
	}
	int TextBox::l_setCaretPosition(lua_State *L) {
		unsigned int v = static_cast<unsigned int>(luaL_checknumber(L, 1));
		gcn::TextBox::setCaretPosition(v);
		return 0;
	}
	int TextBox::l_getText(lua_State *L) {
		const std::string & sr = gcn::TextBox::getText();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int TextBox::l_setText(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::TextBox::setText(v);
		return 0;
	}
	int TextBox::l_getNumberOfRows(lua_State *L) {
		lua_pushnumber(L, gcn::TextBox::getNumberOfRows());
		return 1;
	}
	int TextBox::l_getTextRow(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		const std::string & s = gcn::TextBox::getTextRow(v);
		lua_pushstring(L, s.c_str());
		return 1;
	}
	int TextBox::l_setTextRow(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		const char* s = luaL_checkstring(L, 2);
		gcn::TextBox::setTextRow(v, s);
		return 0;
	}
	int TextBox::l_addRow(lua_State *L) {
		const char* s = luaL_checkstring(L, 1);
		gcn::TextBox::addRow(s);
		return 0;
	}
	int TextBox::l_setOpaque(lua_State *L) {
		bool _opaque = bool(lua_toboolean(L, 1));
		gcn::TextBox::setOpaque(_opaque);
		return 0;
	}
	int TextBox::l_isOpaque(lua_State *L) {
		bool _opaque = gcn::TextBox::isOpaque();
		lua_pushboolean(L, _opaque);
		return 1;
	}
	int TextBox::l_setEditable(lua_State *L) {
		bool _opaque = bool(lua_toboolean(L, 1));
		gcn::TextBox::setEditable(_opaque);
		return 0;
	}
	int TextBox::l_isEditable(lua_State *L) {
		bool _opaque = gcn::TextBox::isEditable();
		lua_pushboolean(L, _opaque);
		return 1;
	}

	const char TextBox::className[] = "TextBox";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<TextBox>::RegType TextBox::methods[] = {
		method(TextBox, getCaretPosition),
		method(TextBox, setCaretPosition),
		method(TextBox, getText),
		method(TextBox, setText),
		method(TextBox, addRow),
		method(TextBox, isOpaque),
		method(TextBox, setOpaque),
		method(TextBox, isEditable),
		method(TextBox, setEditable),
		method(TextBox, getNumberOfRows),
		method(TextBox, getTextRow),
		method(TextBox, setTextRow),
		method(TextBox, setFont),

		LUAGUI_WIDGET_DEF(TextBox),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
