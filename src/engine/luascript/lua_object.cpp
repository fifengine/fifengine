#include "lua_object.h"
#include "object.h"

namespace FIFE {
	Object_LuaScript::Object_LuaScript(Object *obj) {
		//m_object = static_cast<Object*>(lua_touserdata(L, 1));
		m_object = obj;
	}
	Object_LuaScript::~Object_LuaScript() {
	}

	int Object_LuaScript::getPosition(lua_State* L) {
		lua_pushnumber(L, m_object->getPosition());
		return 1;
	}
	int Object_LuaScript::setPosition(lua_State* L) {
		int v = int(luaL_checknumber(L, 1));
		m_object->setPosition(v);
		return 0;
	}
	int Object_LuaScript::getTypeName(lua_State* L) {
		const std::string & name = m_object->getName();
		lua_pushstring(L, name.c_str());
		return 1;
	}

	const char Object_LuaScript::className[] = "Object";

#define method(class, name) {#name, &class::name}
	Lunar<Object_LuaScript>::RegType Object_LuaScript::methods[] = {
		method(Object_LuaScript, getPosition),
		method(Object_LuaScript, setPosition),
		method(Object_LuaScript, getTypeName),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
