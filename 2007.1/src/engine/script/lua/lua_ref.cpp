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
#include "debugutils.h"

#include "lua_ref.h"

namespace FIFE {

	LuaRef* LuaRef::m_list = 0;

	LuaRef::LuaRef() : m_ref(LUA_NOREF),m_state(0) {
		
		// Insert 'this' into list of all LuaRef's
		if( m_list == 0 ) {
			m_prev = m_succ = 0;
		} else {
			m_list->m_prev = this;
			m_succ = m_list;
			m_prev = 0;
		}
		m_list = this;

	}

	LuaRef::~LuaRef() {
		unref();

		// Remove 'this' from list of all LuaRef's
		if( m_succ ) {
			m_succ->m_prev = m_prev;
		}

		if( m_prev ) {
			m_prev->m_succ = m_succ;
		} else {
			// No 'prev' -> we are first element, and thus m_list.
			m_list = m_succ;
		}
	}

	void LuaRef::unrefAll(lua_State* L) {
		LuaRef* lua_ref = m_list;
		while( lua_ref ) {
			if( lua_ref->m_state == L ) {
				lua_ref->unref();
			}
			lua_ref = lua_ref->m_succ;
		}
	}

	void LuaRef::ref(lua_State *L, int index) {
		unref();

		lua_pushvalue(L, index);
		m_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		m_state = L;
	}

	void LuaRef::unref() {
		if( isValid() ) {
			luaL_unref(m_state, LUA_REGISTRYINDEX, m_ref);
			m_state = 0;
			m_ref = LUA_NOREF;
		}
	}

	bool LuaRef::isValid() const {
		return m_ref != LUA_NOREF;
	}

	void LuaRef::push() {
		assert( isValid() );
		lua_rawgeti(m_state, LUA_REGISTRYINDEX, m_ref);
	}

	lua_State* LuaRef::getState() {
		assert( isValid() );
		return m_state;
	}

}
