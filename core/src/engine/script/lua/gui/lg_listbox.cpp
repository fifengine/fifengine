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

	ListBox::ListBox(lua_State *L) : AWidget(), 
		gcn::ListBox(static_cast<userdataType*>(luaL_checkudata(L, 1, 
					luaGui::ListModel::className))->pT) {
		adjustSize();
	}
	ListBox::~ListBox() {}
	int ListBox::l_setListModel(lua_State *L) {
		gcn::ListBox::setListModel(static_cast<userdataType*>(luaL_checkudata(L, 1, 
					luaGui::ListModel::className))->pT);
		return 0;
	}
	int ListBox::l_getSelected(lua_State *L) {
		int k = getSelected();
		lua_pushnumber(L, k);
		return 1;
	}
	int ListBox::l_setSelected(lua_State *L) {
		int k = int(luaL_checknumber(L, 1));
		gcn::ListBox::setSelected(k);
		return 0;
	}

	const char ListBox::className[] = "ListBox";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<ListBox>::RegType ListBox::methods[] = {
		method(ListBox, getSelected),
		method(ListBox, setSelected),
		method(ListBox, setListModel),
		method(ListBox, setFont),

		LUAGUI_WIDGET_DEF(ListBox),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
