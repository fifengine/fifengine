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
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log.h"

#include "luagui.h"
// for lua2gcn_cast
#include "lua_guimanager.h"

namespace luaGui {

	ScrollArea::ScrollArea(lua_State *L) {
	}

	ScrollArea::~ScrollArea() {
		// just to be safe
	}

	int ScrollArea::l_setContent(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::ScrollArea") << "Cannot add a NULL pointer!";
			return 0;
		}
		//FIFE::ActionListener_Lua * al = FIFE::ActionListener_Lua::instance();
		//w->addActionListener(al);
		gcn::ScrollArea::setContent(w);
		return 0;
	}

	int ScrollArea::l_setScrollPolicy(lua_State *L) {
		unsigned int h = int(luaL_checknumber(L, 1));
		unsigned int v = int(luaL_checknumber(L, 2));
		gcn::ScrollArea::setScrollPolicy(h, v);
		return 0;
	}
	
	int ScrollArea::l_getScrollPolicy(lua_State *L) {
		unsigned int h = gcn::ScrollArea::getHorizontalScrollPolicy();
		unsigned int v = gcn::ScrollArea::getVerticalScrollPolicy();
		lua_pushnumber(L, h);
		lua_pushnumber(L, v);
		return 2;
	}

	int ScrollArea::l_setScrollbarWidth(lua_State *L) {
		int w = int(luaL_checknumber(L, 1));
		gcn::ScrollArea::setScrollbarWidth(w);
		return 0;
	}

	int ScrollArea::l_setScrollAmount(lua_State *L) {
		int h = int(luaL_checknumber(L, 1));
		int v = int(luaL_checknumber(L, 2));
		gcn::ScrollArea::setScrollAmount(h, v);
		return 0;
	}

	int ScrollArea::l_getMaxScroll(lua_State *L) {
		int v = gcn::ScrollArea::getVerticalMaxScroll();
		int h = gcn::ScrollArea::getHorizontalMaxScroll();
		lua_pushnumber(L, h);
		lua_pushnumber(L, v);
		return 2;
	}

	const char ScrollArea::className[] = "ScrollArea";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<ScrollArea>::RegType ScrollArea::methods[] = {
		method(ScrollArea, getScrollPolicy),
		method(ScrollArea, setScrollPolicy),
		method(ScrollArea, setScrollbarWidth),
		method(ScrollArea, setScrollAmount),
		method(ScrollArea, getMaxScroll),
		method(ScrollArea, setContent),

		LUAGUI_WIDGET_DEF(ScrollArea),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
