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
