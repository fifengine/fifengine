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

#ifndef FIFE_SCRIPT_LUA_LUA_MAP_H
#define FIFE_SCRIPT_LUA_LUA_MAP_H

// Standard C++ library includes

// 3rd party library includes
#include <boost/shared_ptr.hpp>
#include "lua.hpp"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/lunar.h"
#include "script/lua_table.h"

namespace FIFE {

	namespace map {
		class Map;
		typedef boost::shared_ptr<Map> MapPtr;
	}

	class Map_LuaScript : public Table_LuaScript {
		public:
			static const char className[];
			static Lunar<Map_LuaScript>::RegType methods[];
			static Lunar<Map_LuaScript>::RegType metamethods[];

			Map_LuaScript(lua_State *L);
			Map_LuaScript(map::MapPtr obj);
			virtual ~Map_LuaScript();

			int getElevation(lua_State* L);
			int addElevation(lua_State* L);
			int getNumElevations(lua_State* L);
			int insertElevation(lua_State* L);
			int removeElevation(lua_State* L);

			virtual Table* getTable();

			map::MapPtr getMap() { return m_map; }

		private:
			map::MapPtr m_map;
	};

}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
