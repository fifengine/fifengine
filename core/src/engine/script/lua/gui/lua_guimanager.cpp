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
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/gui/guimanager.h"
#include "util/log.h"

#include "luagui.h"
#include "lua_gui_actionlistener.h"
#include "lua_guimanager.h"

namespace {
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
}

namespace FIFE {

	//private function (dont use other than in lua2gcn_cast) to reduce code copy&pastage (i hate copy&paste) useless_comment++
	//function assumes that there is already one metatable on the lua stack
	//returns either the casted type or 0, incase of the casted type it also cleans up the already existing metatable
	template<typename T> gcn::Widget* lua2gcn_cast_helper(lua_State* L, const char* classname) {
		lua_getfield(L, LUA_REGISTRYINDEX, classname); //get the metatable associated with the typename
		if(lua_rawequal(L, -1, -2)){ //compare metatables and with them the type
			lua_pop(L, 2); //remove metatables as they arent needed anylonger (both!)
			void* p= luaL_checkudata(L, 1, classname);
			T* u = static_cast<T*>(p);
			return dynamic_cast<gcn::Widget*>(u->pT);
		}
		lua_pop(L, 1); //remove the metatable from the last comparison
		return 0;
	}


	gcn::Widget * GuiManager_LuaScript::lua2gcn_cast(lua_State *L) {
		gcn::Widget* p;
		if(!lua_getmetatable(L, 1)){//check if the object has a metatable, if not it is an unknown object
			return 0;
		}
		if((p= lua2gcn_cast_helper<ud_B>(L, luaGui::Button::className))) return p;
		if((p= lua2gcn_cast_helper<ud_TB>(L, luaGui::TwoButton::className))) return p;
		if((p= lua2gcn_cast_helper<ud_L>(L, luaGui::Label::className))) return p;
		if((p= lua2gcn_cast_helper<ud_T>(L, luaGui::TextField::className))) return p;
		if((p= lua2gcn_cast_helper<ud_I>(L, luaGui::Icon::className))) return p;
		if((p= lua2gcn_cast_helper<ud_C>(L, luaGui::Container::className))) return p;
		if((p= lua2gcn_cast_helper<ud_CB>(L, luaGui::CheckBox::className))) return p;
		if((p= lua2gcn_cast_helper<ud_S>(L, luaGui::ScrollArea::className))) return p;
		if((p= lua2gcn_cast_helper<ud_TX>(L, luaGui::TextBox::className))) return p;
		if((p= lua2gcn_cast_helper<ud_LB>(L, luaGui::ListBox::className))) return p;
		if((p= lua2gcn_cast_helper<ud_D>(L, luaGui::DropDown::className))) return p;
		if((p= lua2gcn_cast_helper<ud_R>(L, luaGui::RadioButton::className))) return p;
		if((p= lua2gcn_cast_helper<ud_SL>(L, luaGui::Slider::className))) return p;
		if((p= lua2gcn_cast_helper<ud_W>(L, luaGui::Window::className))) return p;
		if((p= lua2gcn_cast_helper<ud_CL>(L, luaGui::ClickLabel::className))) return p;
		lua_pop(L, 1); //remove the metatable

		return 0;
	}

	LuaRef GuiManager_LuaScript::m_font;

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
		lua_pushvalue(L, 1);
		FIFE::LuaRef* lref = new FIFE::LuaRef();
		lref->ref(L, 1);
		//FIXME: can overwrite existing ptr!
		m_child_refs[_w] = luaGui::LuaRefPtr( lref );

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
		m_child_refs.erase(_w);
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

	int GuiManager_LuaScript::setGlobalFont(lua_State* L) {
		if( lua_gettop(L) == 0 ) {
			GUIManager::instance()->setGlobalFont(0);
			m_font.unref();
		} else {
			gcn::Font *font = luaGui::AWidget::lua2font_cast(L);
			m_font.ref(L,1);
			GUIManager::instance()->setGlobalFont(font);
		}
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
		Lunar<luaGui::ImageFont>::Register(L);
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
		method(setGlobalFont, setGlobalFont),
		{NULL, NULL}
	};

	luaGui::Widget2RefPtrMap GuiManager_LuaScript::m_child_refs;
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
