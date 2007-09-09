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

#include <guichan.hpp>
#include "log.h"
#include "luagui.h"
// for lua2gcn_cast
#include "lua_guimanager.h"
// to forward events
#include "lua_gui_actionlistener.h"

namespace luaGui {
	Container::Container(lua_State *L) {
	}

	Container::~Container() {
	}

	int Container::l_moveToTop(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Container") << "Cannot moveToTop a NULL pointer!";
			return 0;
		}
		gcn::Container::moveToTop(w);
		return 0;
	}

	int Container::l_moveToBottom(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Container") << "Cannot moveToBottom a NULL pointer!";
			return 0;
		}
		gcn::Container::moveToBottom(w);
		return 0;
	}

	int Container::l_add(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Container") << "Cannot add a NULL pointer!";
			return 0;
		}
		FIFE::ActionListener_Lua * al = FIFE::ActionListener_Lua::instance();
		w->addActionListener(al);
		gcn::Container::add(w);
		return 0;
	}

	int Container::l_remove(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Container") << "Cannot remove a NULL pointer!";
			return 0;
		}
		gcn::Container::remove(w);
		return 0;
	}

	int Container::l_clear(lua_State *L) {
		gcn::Container::clear();
		return 0;
	}

	int Container::l_setOpaque(lua_State *L) {
		bool _opaque = bool(lua_toboolean(L, 1));
		gcn::Container::setOpaque(_opaque);
		return 0;
	}

	int Container::l_isOpaque(lua_State *L) {
		bool _opaque = gcn::Container::isOpaque();
		lua_pushboolean(L, _opaque);
		return 1;
	}

	const char Container::className[] = "Container";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Container>::RegType Container::methods[] = {
		method(Container, moveToTop),
		method(Container, moveToBottom),
		method(Container, setOpaque),
		method(Container, isOpaque),
		method(Container, add),
		method(Container, remove),
		method(Container, clear),

		LUAGUI_WIDGET_DEF(Container),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
