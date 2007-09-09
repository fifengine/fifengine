#include "lua_console.h"
#include "console.h"
#include "engine.h"
#include "gui.h"

namespace FIFE
{

	int Console_LuaScript::print(lua_State* L) {
		std::string msg(luaL_checkstring(L, 1));
		Console* console = dynamic_cast<Console*>(Engine::instance()->getGUI()->getWidget("console"));
		if( console ) console->print(msg);
		return 0;
	}
	int Console_LuaScript::attach(lua_State* L) {
		Console* console = dynamic_cast<Console*>(Engine::instance()->getGUI()->getWidget("console"));
		if( console ) console->attach();
		return 0;
	}
	int Console_LuaScript::detach(lua_State* L) {
		Console* console = dynamic_cast<Console*>(Engine::instance()->getGUI()->getWidget("console"));
		if( console ) console->detach();
		return 0;
	}
	
	int luaopen_console(lua_State* L) {
		luaL_openlib(L, "console", Console_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, Console_LuaScript::name}

	const luaL_reg Console_LuaScript::methods[] = {
		method(print),
		method(attach),
		method(detach),
		{NULL, NULL}
	};
}
