#include <assert.h>
#include <string.h>
#include <list>
#include "luascript.h"
#include "lua_mapview.h"
#include "lua_object.h"
#include "lua_log.h"
#include "mapelevation.h"
#include "object.h"
#include "lunar.h"

namespace FIFE {
	extern int luaopen_mapview(lua_State* L);
	extern int luaopen_console(lua_State* L);
	extern int luaopen_log(lua_State* L);
	extern int luaopen_audiomanager(lua_State *L);

	void LuaScript::_initLua() {
		L = lua_open();
		if (L == NULL)
			throw FIFE::ScriptException(implementationName + 
					" - Creating LUA state failed");
		implementationName = LUA_VERSION;

		//FIXME: hardcoded
		luaopen_base(L);
		luaopen_io(L);
		luaopen_string(L);
		luaopen_math(L);
		luaopen_log(L);
		luaopen_mapview(L);
		luaopen_console(L);
		luaopen_audiomanager(L);
		Lunar<Object_LuaScript>::Register2(L);
	}

	LuaScript::LuaScript() {
		Log("LuaScript") << "scripting enabled";
		L = NULL;
	}

	void LuaScript::start(const std::string scriptFile) {
		if (L != NULL) {
			Log("LuaScript") << "state already started? Stopping it first!";
			stop();
		}
		_initLua();
		runFile(scriptFile);
	}

	void LuaScript::stop() {
		if (L != NULL) {
			lua_close(L);
			L = NULL;
			Log("LuaScript") << "scripting disabled";
		}
	}

	LuaScript::~LuaScript() {
		stop(); // just in case
	}

	/* now the base calls from c++ to the script layer ... */

	void LuaScript::runFile(const std::string filename) {
		if (filename.size() > 0) {
			Log("LuaScript") << implementationName << std::string("loading file: ") << filename;
			if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
				throw FIFE::ScriptException(lua_tostring(L,-1));
		}
	}

	void LuaScript::runString(const char *s) {
		if (*s != 0) {
			Log("LuaScript") << implementationName << std::string("loading string:") << std::string(s);
			if (luaL_loadbuffer(L, s, strlen(s), "cmd") || lua_pcall(L, 0, 0, 0))
				throw FIFE::ScriptException(lua_tostring(L,-1));
		}
	}

	void LuaScript::setGlobalInt(const std::string name, const int val) {
		lua_pushnumber(L, val);
		lua_setglobal(L, name.c_str());
	}

	void LuaScript::setGlobalDouble(const std::string name, const double val) {
		lua_pushnumber(L, val);
		lua_setglobal(L, name.c_str());
	}

	void LuaScript::setGlobalString(const std::string name, const std::string val) {
		lua_pushstring(L, val.c_str());
		lua_setglobal(L, name.c_str());
	}

	int LuaScript::getGlobalInt(const std::string name) {
		int result = 0;
		lua_getglobal(L, name.c_str());
		if (!lua_isnumber(L, -1))
			throw FIFE::ScriptException("INT value expected in getGlobalInt("+name+")");
		result = int(lua_tonumber(L, -1)); 
		lua_pop(L, 1);
		return result;
	}

	double LuaScript::getGlobalDouble(const std::string name) {
		double result = 0.0f;
		lua_getglobal(L, name.c_str());
		if (!lua_isnumber(L, -1))
			throw FIFE::ScriptException("DOUBLE value expected in getGlobalDouble("+name+")");
		result = lua_tonumber(L, -1); 
		lua_pop(L, 1);
		return result;
	}

	const char* LuaScript::getGlobalString(const std::string name) {
		const char* result = NULL;
		lua_getglobal(L, name.c_str());
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException("STRING value expected in getGlobalString("+name+")");
		result = lua_tostring(L, -1); 
		lua_pop(L, 1);
		return result;

	}

	void LuaScript::registerMapViewInstance(MapView* currentMap) {
		MapView_LuaScript::registerMap(currentMap);

	}

	void LuaScript::setActiveMapElevation(size_t id) {
		MapView* mv = MapView_LuaScript::getMapView();
		assert(mv!=NULL);
		lua_getglobal(L, "map");
		if (lua_istable(L, -1)) {
			Log("luascript") << "Info: already have loaded map - deleting";
			lua_pop(L, 1);
			lua_pushnil(L);
			lua_setglobal(L, "map");
			// run gc ?
		}
		lua_newtable(L);
		lua_setglobal(L, "map");
		// make sure it is on top
		lua_getglobal(L, "map");

		MapElevation * me = mv->getCurrentElevation();
		assert(me!=NULL);
		std::list<Object*> obj_list = me->getObjects();
		std::list<Object*>::iterator i = obj_list.begin();
		int c = 0;
		while (i != obj_list.end()) {
			//lua_pushlightuserdata(L, static_cast<void*>(*i));
			Object_LuaScript * lo = new Object_LuaScript(*i);
			lua_pushnumber(L, c);
			//lua_pushlightuserdata(L, static_cast<void*>(lo));
			Lunar<Object_LuaScript>::push(L, lo, true);
			lua_settable(L, -3);
			i++;
			c++;
		}
	}

	/* now the calls from the scripts to c++ */

	/*void LuaScript::activateGameState(const std::string name) {
		if (gsm->gameStateExists(name))
		gsm->activate(name);
		}*/

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
