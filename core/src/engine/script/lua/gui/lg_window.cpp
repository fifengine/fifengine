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
#include "util/debugutils.h"
#include "log.h"

#include "luagui.h"
// for lua2gcn_cast
#include "lua_guimanager.h"
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "lua_gui_actionlistener.h"
#endif

namespace luaGui {

	Window::Window(lua_State *L) : AWidget(), gcn::Window() {
		if (lua_gettop(L)) {
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
			gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
			if (w) {
				lua_pushvalue(L, 1);
				FIFE::LuaRef* lref = new FIFE::LuaRef();
				lref->ref(L, 1);
				//FIXME: can overwrite existing ptr!
				m_child_refs[w] = LuaRefPtr( lref );

				gcn::Container::add(w);
			}
#else
			gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
			if (w)
				gcn::Window::setContent(w);
#endif
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
#if GUICHAN_VERSION == 4
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w)
			gcn::Window::setContent(w);
		resizeToContent();
#else
		FIFE::Log("luaGui::Window", FIFE::Log::LEVEL_WARN) << 
			"Backward-compatibility hack engaged - you _really_ should use :add on Windows";
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w) {
			lua_pushvalue(L, 1);
			FIFE::LuaRef* lref = new FIFE::LuaRef();
			lref->ref(L, 1);
			//FIXME: can overwrite existing ptr!
			m_child_refs[w] = LuaRefPtr( lref );

			gcn::Container::add(w);

			gcn::Container::clear();
			gcn::Container::add(w);
		}
#endif

		return 0;
	}

// new with guichan 0.5
// simple copy-n-paste from lg_container
// yes, I know I really shouldn't do that :-)
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
	int Window::l_moveToTop(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Window") << "Cannot moveToTop a NULL pointer!";
			return 0;
		}
		gcn::Container::moveToTop(w);
		return 0;
	}

	int Window::l_moveToBottom(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Window") << "Cannot moveToBottom a NULL pointer!";
			return 0;
		}
		gcn::Container::moveToBottom(w);
		return 0;
	}

	int Window::l_add(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Window") << "Cannot add a NULL pointer!";
			return 0;
		}
		lua_pushvalue(L, 1);
		FIFE::LuaRef* lref = new FIFE::LuaRef();
		lref->ref(L, 1);
		//FIXME: can overwrite existing ptr!
		m_child_refs[w] = LuaRefPtr( lref );
		FIFE::ActionListener_Lua * al = FIFE::ActionListener_Lua::instance();
		w->addActionListener(al);
		gcn::Container::add(w);
		return 0;
	}

	int Window::l_remove(lua_State *L) {
		gcn::Widget *w = FIFE::GuiManager_LuaScript::lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("luaGui::Window") << "Cannot remove a NULL pointer!";
			return 0;
		}
		m_child_refs.erase(w);
		gcn::Container::remove(w);
		return 0;
	}

	int Window::l_clear(lua_State *L) {
		gcn::Container::clear();
		return 0;
	}
#endif

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
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
		method(Window, moveToTop),
		method(Window, moveToBottom),
		method(Window, add),
		method(Window, remove),
		method(Window, clear),
#endif

		LUAGUI_WIDGET_DEF(Window),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
