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
#include "log.h"

#include "luagui.h"

namespace luaGui {
	
	CheckBox::CheckBox(lua_State *L) : AWidget(), gcn::CheckBox("", 0) {
		adjustSize();
	}
	CheckBox::~CheckBox() {}
	int CheckBox::l_isMarked(lua_State *L) {
		lua_pushboolean(L, gcn::CheckBox::isMarked());
		return 1;
	}
	int CheckBox::l_setMarked(lua_State *L) {
		bool v = bool(lua_isboolean(L, 1));
		gcn::CheckBox::setMarked(v);
		return 0;
	}
	int CheckBox::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::CheckBox::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int CheckBox::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::CheckBox::setCaption(v);
		adjustSize();
		return 0;
	}

	const char CheckBox::className[] = "CheckBox";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<CheckBox>::RegType CheckBox::methods[] = {
		method(CheckBox, getCaption),
		method(CheckBox, setCaption),
		method(CheckBox, isMarked),
		method(CheckBox, setMarked),
		method(CheckBox, setFont),

		LUAGUI_WIDGET_DEF(CheckBox),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
