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
#include "map/structures/objectinfo.h"
#include "map/factory.h"

#include "lua_object.h"

namespace FIFE {


	Object_LuaScript::Object_LuaScript(map::ObjectPtr obj) : m_object(obj) {
	}

	Object_LuaScript::Object_LuaScript(lua_State* L) : m_object(map::ObjectInfo::create()) {
	}

	Object_LuaScript::~Object_LuaScript() {
	}

	int Object_LuaScript::loadPrototype(lua_State*L) {
		const char* protoname = luaL_checkstring(L,1);
		size_t proto_id = map::Factory::instance()->getPrototypeId(protoname);
		if( proto_id == 0 ) {
			luaL_error(L,"invalid/unknown prototype: '%s'",protoname);
		}
		m_object->loadPrototype(proto_id);
		return 0;
	}

	Table* Object_LuaScript::getTable() { 
		return m_object.get();
	}

	const char Object_LuaScript::className[] = "GObject";

#define method(class, name) {#name, &class::name}
	Lunar<Object_LuaScript>::RegType Object_LuaScript::methods[] = {
		method(Object_LuaScript, getAttr),
		method(Object_LuaScript, setAttr),
		method(Object_LuaScript, loadPrototype),
		{0,0}
	};

	Lunar<Object_LuaScript>::RegType Object_LuaScript::metamethods[] = {
		{ "__index", &Object_LuaScript::getAttr },
		{ "__newindex", &Object_LuaScript::setAttr },
		{0,0}
	};

}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
