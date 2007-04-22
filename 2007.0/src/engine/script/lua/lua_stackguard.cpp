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
#include <cassert>

// 3rd party library includes

// FIFE includes
#include "log.h"

#include "lua_stackguard.h"

LuaStackguard::LuaStackguard(lua_State *L, bool take_action) {
	assert(L);
	m_state = L;
	m_act = take_action;
	m_top = lua_gettop(m_state); 
}

LuaStackguard::~LuaStackguard() {
	int now_top = lua_gettop(m_state);
	if (now_top > m_top) {
		FIFE::Log("Lua_Stackguard", FIFE::Log::LEVEL_WARN) <<
		"Stack-integrity warning: " << now_top << " > " << m_top;
		if (m_act)
			lua_settop(m_state, m_top);
	}
}
