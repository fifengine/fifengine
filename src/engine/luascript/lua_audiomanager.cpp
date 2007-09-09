#include <string>
#include "lua_audiomanager.h"
#include "audiomanager.h"
#include "log.h"
#include "exception.h"

namespace FIFE {

	int AudioManager_LuaScript::setVolume(lua_State *L) {
		float tmpvol = float(luaL_checknumber(L, 1));
		AudioManager::instance()->setVolume(tmpvol);
		return 0;
	}

	/*
	int AudioManager_LuaScript::mute(lua_State *L) {
		AudioManager::instance()->mute();
		return 0;
	}

	int AudioManager_LuaScript::unmute(lua_State *L) {
		AudioManager::instance()->unmute();
		return 0;
	}*/

	int AudioManager_LuaScript::setAmbientSound(lua_State *L) {
		const char* as_char_p = lua_tostring(L, 1);
		if (as_char_p == NULL) {
			// should Log() something
			return 0;
		}
		const std::string as_string(as_char_p);
		try {
			AudioManager::instance()->setAmbientSound(as_string);
		}
		catch (NotFound & e) {
			Log("audioman_lua") << "Caught exception on loading music: " << e.getMessage();
		}
		return 0;
	}

	int luaopen_audiomanager(lua_State *L) {
		luaL_openlib(L, "audiomanager", AudioManager_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, AudioManager_LuaScript::name}

	const luaL_reg AudioManager_LuaScript::methods[] = {
		method(setVolume),/*
		method(mute),
		method(unmute),*/
		method(setAmbientSound),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
