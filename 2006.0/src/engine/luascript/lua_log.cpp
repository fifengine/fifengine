#include "lua_log.h"
#include "log.h"

namespace FIFE
{
	/*
	int Log_LuaScript::addVisibleType(lua_State* L) {
		std::string msg(luaL_checkstring(L, 1));
		Log::addVisibleType(msg);
		return 0;
	}
	int Log_LuaScript::removeVisibleType(lua_State* L) {
		std::string msg(luaL_checkstring(L, 1));
		Log::removeVisibleType(msg);
		return 0;
	}
	int Log_LuaScript::isVisibleType(lua_State* L) {
		std::string msg(luaL_checkstring(L, 1));
		lua_pushboolean(L, Log::isVisibleType(msg));
		return 1;
	}
	*/
	int Log_LuaScript::setLogLevel(lua_State* L) {
		int logLevel = luaL_checkint(L, 1);
		Log::setLogLevel( Log::type_log_level(logLevel) );
		return 0;
	};
	int luaopen_log(lua_State* L) {
		luaL_openlib(L, "log", Log_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, Log_LuaScript::name}

	const luaL_reg Log_LuaScript::methods[] = {
/*
		method(addVisibleType),
		method(removeVisibleType),
		method(isVisibleType),
*/
		method(setLogLevel),
		{NULL, NULL}
	};
}
