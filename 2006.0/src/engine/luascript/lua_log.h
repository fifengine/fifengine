#ifndef LUA_LOG_H
#define LUA_LOG_H
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace FIFE
{
	class Log_LuaScript
	{
		public:
			static const luaL_reg methods[];
			static int setLogLevel(lua_State* L);
			/*
			static int addVisibleType(lua_State* L);
			static int removeVisibleType(lua_State* L);
			static int isVisibleType(lua_State* L);
			*/
	};
}
#endif
