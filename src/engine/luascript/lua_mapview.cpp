#include "lua_mapview.h"

namespace FIFE
{

	int MapView_LuaScript::setXPos(lua_State* L) {
		int xpos = int(luaL_checknumber(L, 1));
		myMap->setXPos(xpos);
		return 0; // number of results
	}
	int MapView_LuaScript::setYPos(lua_State* L) {
		int ypos = int(luaL_checknumber(L, 1));
		myMap->setYPos(ypos);
		return 0; 
	}
	int MapView_LuaScript::getXPos(lua_State* L) {
		lua_pushnumber(L, myMap->getXPos());
		return 1;
	}
	int MapView_LuaScript::getYPos(lua_State* L) {
		lua_pushnumber(L, myMap->getYPos());
		return 1;
	}
	int MapView_LuaScript::setRoofVisible(lua_State* L) {
		bool tmp = bool(lua_toboolean(L, 1));
		myMap->setRoofVisible(tmp);
		return 0;
	}
	int MapView_LuaScript::getRoofVisible(lua_State* L) {
		lua_pushboolean(L, myMap->getRoofVisible());
		return 1;
	}
	int MapView_LuaScript::setRoofTransparent(lua_State* L) {
		bool tmp = bool(lua_toboolean(L, 1));
		myMap->setRoofVisible(tmp);
		return 0;
	}
	int MapView_LuaScript::getRoofTransparent(lua_State* L) {
		lua_pushboolean(L, myMap->getRoofTransparent());
		return 1;
	}
	int MapView_LuaScript::setRoofAlpha(lua_State* L) {
		int tmp = int(luaL_checknumber(L, 1));
		myMap->setRoofAlpha(tmp);
		return 0;
	}
	int MapView_LuaScript::getRoofAlpha(lua_State* L) {
		lua_pushnumber(L, int(myMap->getRoofAlpha()));
		return 1;
	}
	int MapView_LuaScript::setObjectsVisible(lua_State* L) {
		bool tmp = bool(lua_toboolean(L, 1));
		myMap->setObjectsVisible(tmp);
		return 0;
	}
	int MapView_LuaScript::getObjectsVisible(lua_State* L) {
		lua_pushboolean(L, myMap->getObjectsVisible());
		return 1;
	}
	int MapView_LuaScript::setFloorVisible(lua_State* L) {
		bool tmp = bool(lua_toboolean(L, 1));
		myMap->setFloorVisible(tmp);
		return 0;
	}
	int MapView_LuaScript::getFloorVisible(lua_State* L) {
		lua_pushboolean(L, myMap->getFloorVisible());
		return 1;
	}
	int MapView_LuaScript::getCurrentElevation(lua_State* L) {
		lua_pushnumber(L, myMap->getElevationNum());
		return 1;
	}
	int MapView_LuaScript::getElevationCount(lua_State* L) {
		lua_pushnumber(L, myMap->getElevationCount());
		return 1;
	}
	int MapView_LuaScript::setCurrentElevation(lua_State* L) {
		unsigned int val = static_cast<unsigned int>(lua_tonumber(L, 1));
		myMap->setElevationNum(val);
		return 0;
	}

	/* this has to be called from c++ with a pointer
	 * to the current mapview!
	 */
	void MapView_LuaScript::registerMap(MapView* map) {
		MapView_LuaScript::myMap = map;
	}

	MapView* MapView_LuaScript::getMapView() {
		return myMap;
	}


	int luaopen_mapview(lua_State* L) {
		luaL_openlib(L, "mapview", MapView_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, MapView_LuaScript::name}

	MapView* MapView_LuaScript::myMap = NULL;
	const luaL_reg MapView_LuaScript::methods[] = {
		method(getXPos),
		method(getYPos),
		method(setXPos),
		method(setYPos),
		method(setRoofVisible),
		method(getRoofVisible),
		method(setRoofTransparent),
		method(getRoofTransparent),
		method(setRoofAlpha),
		method(getRoofAlpha),
		method(setObjectsVisible),
		method(getObjectsVisible),
		method(setFloorVisible),
		method(getFloorVisible),
		method(getCurrentElevation),
		method(setCurrentElevation),
		method(getElevationCount),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
