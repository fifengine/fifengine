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
#include <string>

// 3rd party library includes
#include "lua.hpp"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "attributedclass.h"

#include "lua_table.h"

namespace FIFE {

	Table_LuaScript::Table_LuaScript() {}
	Table_LuaScript::~Table_LuaScript() {}

	int Table_LuaScript::getAttr(lua_State* L) {
		std::string key = lua_tostring(L,1);

// 		lua_getglobal(L,self->classname().c_str());
// 		lua_pushstring(L,key.c_str());
// 		lua_gettable(L,-2);
// 		lua_remove(L,-2);
// 		if( !lua_isnil(L,-1) )
// 			return 1;
// 		lua_pop(L,1);

		AttributedClass *t = getTable();
		if (!t) {
			lua_pushnil(L);
			return 1;
		}

		if (!t->hasAttribute(key)) {
			lua_pushnil(L);
			return 1;
		}

		if( typeid(key) == t->getTypeInfo(key) ) {
			std::string value = t->get<std::string>(key);
			lua_pushstring(L,value.c_str());
			return 1;
		}

		if( typeid(long(1)) == t->getTypeInfo(key) ) {
			long value = t->get<long>(key);
			lua_pushnumber(L,value);
			return 1;
		}

		Debug("LuaTable")
			<< "get failed to determine lua type of  "
			<< t->getTypeInfo(key).name();

		lua_pushnil(L);
		return 1;
	}

	int Table_LuaScript::setAttr(lua_State* L) {
		AttributedClass *t = getTable();
		if (!t)
			return 0;

		std::string key = lua_tostring(L,1);

		switch(lua_type(L,2)) {
		case LUA_TNUMBER:
			{
				long number = lua_tointeger(L,2);
				lua_pop(L,1);
				t->set<long>(key,number);
				return 0;
			}
		case LUA_TSTRING:
			t->set<std::string>(key,lua_tostring(L,2));
			lua_pop(L,1);
			return 0;
		}
	
		std::string value = lua_tostring(L,2);
		lua_pop(L,1);
		t->set<std::string>(key,value);
		return 0;
	}
}
