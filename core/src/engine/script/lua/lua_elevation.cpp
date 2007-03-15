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
#include "map/elevation.h"

#include "lua_elevation.h"
#include "lua_layer.h"

namespace FIFE {


	Elevation_LuaScript::Elevation_LuaScript(map::ElevationPtr obj) : m_elevation(obj) {
	}

	Elevation_LuaScript::Elevation_LuaScript(lua_State* L) : m_elevation(new map::Elevation()) {
	}

	Elevation_LuaScript::~Elevation_LuaScript() {
	}

	int Elevation_LuaScript::getLayer(lua_State*L) {
		int id = int(luaL_checkinteger(L,1));
		if( id < 0 || id > m_elevation->getNumLayers() ) {
			luaL_error(L,"no layer %d - max layer nr is %d",
			           id,m_elevation->getNumLayers());
		}
		Lunar<Layer_LuaScript>::push(L,new Layer_LuaScript(m_elevation->getLayer(id)));
		return 1;
	}

	int Elevation_LuaScript::addLayer(lua_State*L) {
		map::LayerPtr layer = Lunar<Layer_LuaScript>::check(L,1)->getLayer();
		m_elevation->addLayer( layer );
		return 0;
	}

	Table* Elevation_LuaScript::getTable() { 
		return m_elevation.get();
	}

	const char Elevation_LuaScript::className[] = "Elevation";

#define method(class, name) {#name, &class::name}
	Lunar<Elevation_LuaScript>::RegType Elevation_LuaScript::methods[] = {
		method(Elevation_LuaScript, getAttr),
		method(Elevation_LuaScript, setAttr),
		method(Elevation_LuaScript, getLayer),
		method(Elevation_LuaScript, addLayer),
		{0,0}
	};

	Lunar<Elevation_LuaScript>::RegType Elevation_LuaScript::metamethods[] = {
		{ "__index", &Elevation_LuaScript::getAttr },
		{ "__newindex", &Elevation_LuaScript::setAttr },
		{0,0}
	};


}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
