#ifndef LUA_CONSOLE_H
#define LUA_CONSOLE_H
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

namespace FIFE
{
	class Console_LuaScript
	{
		public:
			static const luaL_reg methods[];
			static int print(lua_State* L);
			static int attach(lua_State* L);
			static int detach(lua_State* L);
	};
}
#endif
