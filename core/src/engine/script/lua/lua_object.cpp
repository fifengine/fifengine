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
#include "lua_object.h"

namespace FIFE {

/*
	Object_LuaScript::Object_LuaScript(Object *obj) {
		//m_object = static_cast<Object*>(lua_touserdata(L, 1));
		m_object = obj;
	}
	Object_LuaScript::~Object_LuaScript() {
	}

	int Object_LuaScript::getPosition(lua_State* L) {
		lua_pushnumber(L, m_object->getPosition());
		return 1;
	}
	int Object_LuaScript::setPosition(lua_State* L) {
		int v = int(luaL_checknumber(L, 1));
		m_object->setPosition(v);
		return 0;
	}
	int Object_LuaScript::getTypeName(lua_State* L) {
		const std::string & name = m_object->getName();
		lua_pushstring(L, name.c_str());
		return 1;
	}

	const char Object_LuaScript::className[] = "Object";

#define method(class, name) {#name, &class::name}
	Lunar<Object_LuaScript>::RegType Object_LuaScript::methods[] = {
		method(Object_LuaScript, getPosition),
		method(Object_LuaScript, setPosition),
		method(Object_LuaScript, getTypeName),
		{0,0}
	};
*/
}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
