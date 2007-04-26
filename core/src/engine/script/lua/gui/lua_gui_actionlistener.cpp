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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/scriptengine.h"
#include "util/debugutils.h"
#include "util/log.h"

#include "lua_gui_actionlistener.h"

namespace FIFE {

	ActionListener_Lua::ActionListener_Lua() {
		isReady = false;
		m_L = NULL;
	}
	ActionListener_Lua::~ActionListener_Lua() {
	}
#if GUICHAN_VERSION == 6
	void ActionListener_Lua::action(const gcn::ActionEvent & event) {
		const std::string & action = event.getId();
#elif GUICHAN_VERSION == 5
	void ActionListener_Lua::action(const std::string &action, gcn::Widget* widget) {
#elif GUICHAN_VERSION == 4
	void ActionListener_Lua::action(const std::string &action) {
#endif
		DEBUG_PRINT("GUI-Action: " << action);
		if (isReady)
			luaAction(action);
		else
			Warn("ActionListener_Lua") << "Warning: received action event while in non-setup state";
	}
	void ActionListener_Lua::setup(lua_State *L, const std::string &global,
			const std::string &table) {
		m_global = global;
		m_table = table;
		m_L = L;
		isReady = true;
	}
	void ActionListener_Lua::luaAction(const std::string &action) {
		lua_getglobal(m_L, m_global.c_str());
		if (!lua_istable(m_L, -1)) {
			Warn("lg_Actionlistener") << "Error: " << m_global << " is not a table";
			lua_pop(m_L, 1);
			return;
		}
		// assert table
		lua_pushstring(m_L, m_table.c_str());
		lua_gettable(m_L, -2);
		if (!lua_istable(m_L, -1)) {
			Warn("lg_Actionlistener") << "Error: sub-element " << m_table << " is not a table";
			lua_pop(m_L, 2);
			return;
		}
		lua_pushstring(m_L, action.c_str());
		lua_gettable(m_L, -2);
		if (!lua_isfunction(m_L, -1)) {
			Warn("lg_Actionlistener") << "Warning: there is no function: " << action;
			lua_pop(m_L, 3);
			return;
		}
		// getfield(L, "actions");
		if (lua_pcall(m_L, 0, 0, 0) != 0) {
			throw ScriptException(lua_tostring(m_L,-1));
		}
		lua_pop(m_L, 2);

	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
