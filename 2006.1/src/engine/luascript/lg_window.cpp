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
// for lua2gcn_cast
#include "lua_guimanager.h"

namespace luaGui {
	Window::Window(lua_State *L) : AWidget(), gcn::Window() {
		if (lua_gettop(L)) {
			gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
			if (w)
				gcn::Window::setContent(w);
		}
		if (lua_gettop(L) == 2)
			gcn::Window::setCaption(luaL_checkstring(L, 2));
		gcn::Window::resizeToContent();
	}
	Window::~Window() {}
	int Window::l_getAlignment(lua_State *L) {
		lua_pushnumber(L, gcn::Window::getAlignment());
		return 1;
	}
	int Window::l_setAlignment(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		gcn::Window::setAlignment(v);
		return 0;
	}
	int Window::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::Window::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int Window::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::Window::setCaption(v);
		return 0;
	}

	int Window::l_setOpaque(lua_State *L) {
		bool _opaque = bool(lua_toboolean(L, 1));
		setOpaque(_opaque);
		return 0;
	}

	int Window::l_isOpaque(lua_State *L) {
		bool _opaque = gcn::Window::isOpaque();
		lua_pushboolean(L, _opaque);
		return 1;
	}
	
	int Window::l_setMovable(lua_State *L) {
		bool _move = bool(lua_toboolean(L, 1));
		setMovable(_move);
		return 0;
	}

	int Window::l_isMovable(lua_State *L) {
		bool _move = gcn::Window::isMovable();
		lua_pushboolean(L, _move);
		return 1;
	}
	int Window::l_getTitleBarHeight(lua_State *L) {
		lua_pushnumber(L, gcn::Window::getTitleBarHeight());
		return 1;
	}
	int Window::l_setTitleBarHeight(lua_State *L) {
		unsigned int v = int(luaL_checknumber(L, 1));
		gcn::Window::setTitleBarHeight(v);
		return 0;
	}
	int Window::l_getPadding(lua_State *L) {
		lua_pushnumber(L, gcn::Window::getPadding());
		return 1;
	}
	int Window::l_setPadding(lua_State *L) {
		unsigned int v = int(luaL_checknumber(L, 1));
		gcn::Window::setPadding(v);
		return 0;
	}
	int Window::l_setContent(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w)
			gcn::Window::setContent(w);
		resizeToContent();

		return 0;
	}


	const char Window::className[] = "Window";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Window>::RegType Window::methods[] = {
		method(Window, getAlignment),
		method(Window, setAlignment),
		method(Window, getCaption),
		method(Window, setCaption),
		method(Window, setMovable),
		method(Window, isMovable),
		method(Window, setOpaque),
		method(Window, isOpaque),
		method(Window, setFont),
		method(Window, getPadding),
		method(Window, setPadding),
		method(Window, getTitleBarHeight),
		method(Window, setTitleBarHeight),
		method(Window, setContent),

		LUAGUI_WIDGET_DEF(Window),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
