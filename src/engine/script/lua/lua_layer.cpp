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
#include "map/layer.h"

#include "lua_layer.h"

namespace FIFE {


	Layer_LuaScript::Layer_LuaScript(map::LayerPtr obj) : m_layer(obj) {
	}

	Layer_LuaScript::Layer_LuaScript(lua_State* L) : m_layer() {
		int x = int(luaL_checknumber(L,1));
		int y = int(luaL_checknumber(L,2));
		int g = int(luaL_checknumber(L,3));
		m_layer = map::LayerPtr(new map::Layer(Point(x,y),g));
	}

	Layer_LuaScript::~Layer_LuaScript() {
	}

	Table* Layer_LuaScript::getTable() { 
		return m_layer.get();
	}

	const char Layer_LuaScript::className[] = "Layer";

#define method(class, name) {#name, &class::name}
	Lunar<Layer_LuaScript>::RegType Layer_LuaScript::methods[] = {
		method(Layer_LuaScript, getAttr),
		method(Layer_LuaScript, setAttr),
		{0,0}
	};

	Lunar<Layer_LuaScript>::RegType Layer_LuaScript::metamethods[] = {
		{ "__index", &Layer_LuaScript::getAttr },
		{ "__newindex", &Layer_LuaScript::setAttr },
		{0,0}
	};


}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
