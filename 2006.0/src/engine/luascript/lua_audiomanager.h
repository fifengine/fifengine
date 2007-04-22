#ifndef LUA_AUDIOMANAGER_H
#define LUA_AUDIOMANAGER_H
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

namespace FIFE {
	class AudioManager_LuaScript {
		public:

			static int setVolume(lua_State *L);
			/*static int mute(lua_State *L);
			static int unmute(lua_State *L);*/

			static int setAmbientSound(lua_State *L);

			static const luaL_reg methods[];
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
