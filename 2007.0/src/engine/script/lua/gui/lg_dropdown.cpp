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
	
	DropDown::DropDown(lua_State *L) : AWidget(), 
		gcn::DropDown(static_cast<userdataType*>(luaL_checkudata(L, 1, 
					luaGui::ListModel::className))->pT) {
			// ugly hack to enforce the sub-listbox to the same size as this
			mListBox->setSize(700, 1);
	}
	DropDown::~DropDown() {}
	int DropDown::l_setListModel(lua_State *L) {
		gcn::DropDown::setListModel(static_cast<userdataType*>(luaL_checkudata(L, 1, 
					luaGui::ListModel::className))->pT);
		return 0;
	}
	int DropDown::l_getSelected(lua_State *L) {
		int k = getSelected();
		lua_pushnumber(L, k);
		return 1;
	}
	int DropDown::l_setSelected(lua_State *L) {
		int k = int(luaL_checknumber(L, 1));
		gcn::DropDown::setSelected(k);
		return 0;
	}

	const char DropDown::className[] = "DropDown";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<DropDown>::RegType DropDown::methods[] = {
		method(DropDown, getSelected),
		method(DropDown, setSelected),
		method(DropDown, setListModel),
		method(DropDown, setFont),

		LUAGUI_WIDGET_DEF(DropDown),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
