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

#include <cassert>
#include <string>
#include "lua_guimanager.h"
#include "video/gui/guimanager.h"
#include "luagui.h"
#include "scriptbase.h"
#include "log.h"
#include "lua_gui_actionlistener.h"
#include "guichan_addon/console.h"

namespace FIFE {
	gcn::Widget * GuiManager_LuaScript::lua2gcn_cast(lua_State *L) {
		typedef struct { luaGui::Button *pT; } ud_B;
		typedef struct { luaGui::TwoButton *pT; } ud_TB;
		typedef struct { luaGui::Label *pT; } ud_L;
		typedef struct { luaGui::TextField *pT; } ud_T;
		typedef struct { luaGui::Icon *pT; } ud_I;
		typedef struct { luaGui::Container *pT; } ud_C;
		typedef struct { luaGui::CheckBox *pT; } ud_CB;
		typedef struct { luaGui::ScrollArea *pT; } ud_S;
		typedef struct { luaGui::TextBox *pT; } ud_TX;
		typedef struct { luaGui::ListBox *pT; } ud_LB;
		typedef struct { luaGui::DropDown *pT; } ud_D;
		typedef struct { luaGui::RadioButton *pT; } ud_R;
		typedef struct { luaGui::Slider *pT; } ud_SL;
		typedef struct { luaGui::Window *pT; } ud_W;
		typedef struct { luaGui::ClickLabel *pT; } ud_CL;

		void * p = NULL;
		gcn::Widget * _w = NULL;

		if ((p = luaL_checkudata(L, 1, luaGui::Button::className))) {
			ud_B * u = static_cast<ud_B*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::TwoButton::className))) {
			ud_TB * u = static_cast<ud_TB*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::Label::className))) {
			ud_L * u = static_cast<ud_L*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::TextField::className))) {
			ud_T * u = static_cast<ud_T*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::Icon::className))) {
			ud_I * u = static_cast<ud_I*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::Container::className))) {
			ud_C * u = static_cast<ud_C*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::CheckBox::className))) {
			ud_CB * u = static_cast<ud_CB*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::ScrollArea::className))) {
			ud_S * u = static_cast<ud_S*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::TextBox::className))) {
			ud_TX * u = static_cast<ud_TX*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::ListBox::className))) {
			ud_LB * u = static_cast<ud_LB*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::DropDown::className))) {
			ud_D * u = static_cast<ud_D*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::RadioButton::className))) {
			ud_R * u = static_cast<ud_R*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::Slider::className))) {
			ud_SL * u = static_cast<ud_SL*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::Window::className))) {
			ud_W * u = static_cast<ud_W*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}
		if ((p = luaL_checkudata(L, 1, luaGui::ClickLabel::className))) {
			ud_CL * u = static_cast<ud_CL*>(p);
			_w = dynamic_cast<gcn::Widget*>(u->pT);
			return _w;
		}

		return _w;
	}


	/** Adds a luaGui derived widget to the gcn top-container.
	 *
	 * This code is pretty boring, but there are two important things:
	 * - luaL_checkudata doesn't support inheritance and the exact metatable
	 *   name is unknown; thus you have to test for each possible case!
	 * - directly casting the luaL_checkudata result (on success) to gcn::Widget
	 *   doesn't work.
	 *   You _have_ to do it in multiple steps.
	 *   @see http://lua-users.org/lists/lua-l/2000-03/msg00035.html
	 */
	int GuiManager_LuaScript::addWidget(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);
		ActionListener_Lua * al = ActionListener_Lua::instance();
		assert(al!=NULL);

		gcn::Widget * _w = lua2gcn_cast(L);

		if (_w == NULL) {
			Warn("lua_Guimanager") << "Error: cannot add pointer to non-luaGui::Widget!";
			return 0;
		}
		_w->addActionListener(al);
		gman->add(_w);
		return 0;
	}
	int GuiManager_LuaScript::removeWidget(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);

		gcn::Widget * _w = lua2gcn_cast(L);

		if (_w == NULL) {
			Warn("lua_Guimanager") << "Error: cannot remove pointer to non-luaGui::Widget!";
			return 0;
		}
		gman->remove(_w);
		return 0;
	}

	int GuiManager_LuaScript::moveWidgetToTop(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);

		gcn::Widget *w = lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("lua_Guimanager") << "Cannot moveToTop a NULL pointer!";
			return 0;
		}
		gman->getTopContainer()->moveToTop(w);
		return 0;
	}

	int GuiManager_LuaScript::moveWidgetToBottom(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);

		gcn::Widget *w = lua2gcn_cast(L);
		if (w == NULL) {
			FIFE::Log("lua_Guimanager") << "Cannot moveToBottom a NULL pointer!";
			return 0;
		}
		gman->getTopContainer()->moveToBottom(w);
		return 0;
	}


	int GuiManager_LuaScript::console_println(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);
		const char* s = static_cast<const char*>(luaL_checkstring(L, 1));
		if (s)
			gman->getConsole()->println(s);
		else
			gman->getConsole()->println("Invalid argument to function console_println!");
		return 0;
	}

	int GuiManager_LuaScript::console_show(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);
		gman->getConsole()->show();
		return 0;
	}
	
	int GuiManager_LuaScript::console_hide(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);
		gman->getConsole()->hide();
		return 0;
	}

	int GuiManager_LuaScript::console_clear(lua_State *L) {
		GUIManager *gman = GUIManager::instance();
		assert(gman!=NULL);
		gman->getConsole()->clear();
		return 0;
	}


	int luaopen_guimanager(lua_State *L) {
		luaL_openlib(L, "guimanager", GuiManager_LuaScript::methods, 0);
		Lunar<luaGui::Image>::Register(L);
		Lunar<luaGui::Button>::Register(L);
		Lunar<luaGui::Label>::Register(L);
		Lunar<luaGui::TextField>::Register(L);
		Lunar<luaGui::Icon>::Register(L);
		Lunar<luaGui::Container>::Register(L);
		Lunar<luaGui::Color>::Register(L);
		Lunar<luaGui::TwoButton>::Register(L);
		Lunar<luaGui::TTFont>::Register(L);
		Lunar<luaGui::CheckBox>::Register(L);
		Lunar<luaGui::ScrollArea>::Register(L);
		Lunar<luaGui::TextBox>::Register(L);
		Lunar<luaGui::ListModel>::Register(L);
		Lunar<luaGui::ListBox>::Register(L);
		Lunar<luaGui::DropDown>::Register(L);
		Lunar<luaGui::RadioButton>::Register(L);
		Lunar<luaGui::Slider>::Register(L);
		Lunar<luaGui::Window>::Register(L);
		Lunar<luaGui::ClickLabel>::Register(L);

		lua_newtable(L);
		lua_pushstring(L, "actions");
		lua_newtable(L);
		lua_settable(L, -3);
		lua_pushstring(L, "anim_end");
		lua_newtable(L);
		lua_settable(L, -3);

		lua_setglobal(L, "gui");

		ActionListener_Lua * al = ActionListener_Lua::instance();
		al->setup(L, "gui", "actions");
		return 0;
	}

#define method(name,realname) {#name, GuiManager_LuaScript::realname}
	const luaL_reg GuiManager_LuaScript::methods[] = {
		method(add,    addWidget),
		method(remove, removeWidget),
		method(moveToTop, moveWidgetToTop),
		method(moveToBottom, moveWidgetToBottom),
		method(console_println, console_println),
		method(console_hide, console_hide),
		method(console_show, console_show),
		method(console_clear, console_clear),
		{NULL, NULL}
	};

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
