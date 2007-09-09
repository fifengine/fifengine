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

#ifndef LUA_MAPVIEW_H
#define LUA_MAPVIEW_H
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "mapview.h"

namespace FIFE
{
	class MapView_LuaScript
	{
		public:
			static const luaL_reg methods[];

			static int setXPos(lua_State* L);
			static int setYPos(lua_State* L);
			static int getXPos(lua_State* L);
			static int getYPos(lua_State* L);
			static int setRoofVisible(lua_State* L);
			static int getRoofVisible(lua_State* L);
			static int setRoofTransparent(lua_State* L);
			static int getRoofTransparent(lua_State* L);
			static int setRoofAlpha(lua_State* L);
			static int getRoofAlpha(lua_State* L);
			static int setObjectsVisible(lua_State* L);
			static int getObjectsVisible(lua_State* L);
			static int setFloorVisible(lua_State* L);
			static int getFloorVisible(lua_State* L);
			static int getCurrentElevation(lua_State* L);
			static int setCurrentElevation(lua_State* L);
			static int getElevationCount(lua_State* L);

			/* this has to be called from c++ with a pointer
			 * to the current mapview!
			 */
			static void registerMap(MapView* map);
			static MapView* getMapView();

		private:
			static MapView* myMap;

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
