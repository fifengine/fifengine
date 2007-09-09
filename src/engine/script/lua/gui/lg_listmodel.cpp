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
#include "luagui.h"

namespace luaGui {

	ListModel::ListModel(lua_State *L) : 
		gcn::ListModel(), elements(lua_gettop(L)) {
			for (int i = 0; i < lua_gettop(L); ++i) {
				elements[i] = luaL_checkstring(L, i);
			}
	}
	ListModel::~ListModel() {}
	int ListModel::l_getNumberOfElements(lua_State *L) {
		lua_pushnumber(L, getNumberOfElements());
		return 1;
	}
	int ListModel::l_getElementAt(lua_State *L) {
		int i = int(luaL_checknumber(L, 1));
		lua_pushstring(L, getElementAt(i).c_str());
		return 1;
	}
	int ListModel::l_addElement(lua_State *L) {
		for (int i = 0; i < lua_gettop(L); ++i) {
			elements.push_back( luaL_checkstring(L, i) );
		}
		return 1;
	}
		
	int ListModel::getNumberOfElements() {
		return elements.size();
	}
	std::string ListModel::getElementAt(int k) {
		return elements[k];
	}
	const char ListModel::className[] = "ListModel";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<ListModel>::RegType ListModel::methods[] = {
		method(ListModel, addElement),
		method(ListModel, getNumberOfElements),
		method(ListModel, getElementAt),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
