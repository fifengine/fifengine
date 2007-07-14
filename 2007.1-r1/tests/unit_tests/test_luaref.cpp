/***************************************************************************
*   Copyright (C) 2005-2007 by the FIFE Team                              *
*   fife-public@lists.sourceforge.net                                     *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
***************************************************************************/

// Standard C++ library includes
#include <stdio.h>
#include <iostream>
#include <cassert>

// 3rd party library includes
#include <boost/test/unit_test.hpp>
#include <lua.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/layer.h"
#include "map/objectinfo.h"
#include "map/factory.h"
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "util/settingsmanager.h"
#include "script/lunar.h"
#include "script/lua_ref.h"


using boost::unit_test::test_suite;
using namespace FIFE::map;
using namespace FIFE;

class DemoObj {
	public:
#define method(name) {#name, DemoObj::name}

	DemoObj(lua_State *L) { 
		std::cout << "instance created" << std::endl;
	}
	
	~DemoObj() {
		std::cout << "instance destroyed" << std::endl;
	}
	
	static const char className[];
	static Lunar<DemoObj>::RegType methods[];
};

const char DemoObj::className[] = "DemoObj";
Lunar<DemoObj>::RegType DemoObj::methods[] = {
	{NULL, NULL}
};


void lua_run(lua_State *L, const char* cmd_buf) {
	if (luaL_loadbuffer(L, cmd_buf, strlen(cmd_buf), "cmd") || lua_pcall(L, 0, 0, 0)) {
		std::cerr << "Error: " << lua_tostring(L, 1) << std::endl;
		lua_pop(L, 1);
		exit(1);
	}
}

struct environment {
	lua_State * state;
	environment() {
		state = lua_open();
		luaL_openlibs(state);
		Lunar<DemoObj>::Register(state);
	}
	~environment() {
		lua_close(state);
	}
};

void simple_test()
{
	// Check whether LuaRef gracefully handles
	// lua_close() though it's still held.

	environment env;
	lua_State *L = env.state;
	
	const char * cmd_buf = "foo = DemoObj()";
	
	lua_run(L, cmd_buf);
	
	lua_getglobal(L, "foo");
	BOOST_CHECK( lua_isuserdata(L, 1) );
	
	BOOST_CHECK( lua_gettop(L) == 1 );
	
	LuaRef luaref;
	luaref.ref(L, 1);
	lua_pop(L, 1);
	
	BOOST_CHECK( lua_gettop(L) == 0);
	
	lua_run(L, "_G[\"foo\"] = nil");
// 	std::cout << lua_gc(L, LUA_GCCOUNT, 0) << lua_gc(L, LUA_GCCOUNTB, 0) << "B\n";
	lua_gc(L, LUA_GCCOLLECT, 0);
// 	std::cout << lua_gc(L, LUA_GCCOUNT, 0) << lua_gc(L, LUA_GCCOUNTB, 0) << "B\n";
	luaref.unref();
	lua_gc(L, LUA_GCCOLLECT, 0);
// 	std::cout << lua_gc(L, LUA_GCCOUNT, 0) << lua_gc(L, LUA_GCCOUNTB, 0) << "B\n";
	lua_run(L, cmd_buf);
	luaref.ref(L, 1);
	lua_pop(L, 1);
	LuaRef::unrefAll(L);
	BOOST_CHECK( !luaref.isValid() );
}

void anonymous_function_test() {
	// Check whether LuaRef is suitable for callbacks.

	environment env;
	lua_State *L = env.state;
	
	LuaRef luaref;
	lua_run(L, "foo = function() print(\"Lambda call works.\") end");
	lua_getglobal(L,"foo");
	luaref.ref(L,1);
	lua_pop(L,1);
	lua_run(L, "_G[\"foo\"] = nil");
	BOOST_CHECK( lua_gettop(L) == 0);
	lua_gc(L, LUA_GCCOLLECT, 0);

	lua_getglobal(L,"foo");
	BOOST_CHECK( lua_isnil(L, 1) );
	lua_pop(L,1);

	luaref.push();
	BOOST_CHECK( lua_isfunction(L, 1) );
	lua_call(L,0,0); // Should print the message

	LuaRef::unrefAll(L);
}

void double_ref_test() {
	environment env;
	
	lua_State *L = env.state;
	
	LuaRef luaref[2];
	lua_run(L, "foo = function() print(\"Double references work.\") end");
	lua_getglobal(L,"foo");
	luaref[0].ref(L,1);
	luaref[1].ref(L,1);
	lua_pop(L,1);
	lua_run(L, "_G[\"foo\"] = nil");
	BOOST_CHECK( lua_gettop(L) == 0);
	lua_gc(L, LUA_GCCOLLECT, 0);

	luaref[0].unref();
	lua_gc(L, LUA_GCCOLLECT, 0);

	// Now one reference was released,
	// but 'foo' should still not be gc'ed.

	luaref[1].push();
	BOOST_CHECK( lua_isfunction(L, 1) );
	lua_call(L,0,0); // Should print the message

	LuaRef::unrefAll(L);
}

test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("Lua Ref tests");
	test->add(BOOST_TEST_CASE(&simple_test), 0);
	test->add(BOOST_TEST_CASE(&anonymous_function_test), 0);
	test->add(BOOST_TEST_CASE(&double_ref_test), 0);
	return test;
}
