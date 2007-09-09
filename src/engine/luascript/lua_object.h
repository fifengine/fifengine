#ifndef LUASCRIPT_OBJECT_H
#define LUASCRIPT_OBJECT_H

// from the example at: http://lua-users.org/wiki/CppObjectBinding

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#include "lunar.h"

namespace FIFE {

	class Object;
	class Object_LuaScript {
		public:
			static const char className[];
			static Lunar<Object_LuaScript>::RegType methods[];

			Object_LuaScript(Object *obj);
			~Object_LuaScript();
			int getPosition(lua_State *L);
			int setPosition(lua_State *L);
			int getTypeName(lua_State *L);
		private:
			Object *m_object;
	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
