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
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/command/info.h"
#include "map/objectinfo.h"
#include "script/lua/lua_stackguard.h"
#include "script/lua/lua_object.h"
#include "script/scriptcontainer.h"
#include "event.h" // FIXME: Needed?
#include "log.h"

#include "scriptingslave.h"

namespace FIFE { namespace map {

	ScriptingSlave* ScriptingSlave::m_instance;
	
	ScriptingSlave::ScriptingSlave() : 
		AsyncBridge(), m_ticks(SDL_GetTicks()), m_sleeping() {

		vm = lua_open();

		lua_pushcfunction(vm, luaopen_base);
		lua_call(vm, 0, 0);

		lua_pushcfunction(vm, luaopen_math);
		lua_call(vm, 0, 0);

		lua_pushcfunction(vm, luaopen_table);
		lua_call(vm, 0, 0);

		lua_pushcfunction(vm, luaopen_package);
		lua_call(vm, 0, 0);

		lua_pushcfunction(vm, luaopen_string);
		lua_call(vm, 0, 0);

		m_instance = this;
		/* 
		* This could be unified to make extension easier...
		* e.g Array of funtions and corresponding lua-names 
		*/
		// Use upvalues: http://www.lua.org/pil/27.3.3.html
		lua_pushcfunction(vm, lua_registerObject);
		lua_setglobal(vm, "FIFE_RegisterObject");
		lua_pushcfunction(vm, lua_execcommand);
		lua_setglobal(vm, "FIFE_ExecCommand");
		lua_pushcfunction(vm, lua_sleep);
		lua_setglobal(vm, "FIFE_Sleep");

		Lunar<Object_LuaScript>::RegisterTable(vm);
	}
	
	ScriptingSlave::~ScriptingSlave() {
		lua_close(vm);
		m_instance = 0;
	}

	void ScriptingSlave::prepareGObjectCall(int ref, const std::string& function) {
		int top = lua_gettop(vm);
		// retrieve the object via ref
		lua_rawgeti(vm, LUA_REGISTRYINDEX, ref);
		// get thememberfunction
		lua_pushstring(vm, "onWake");
		lua_gettable(vm, top + 1);
		// member function is on stack, now push arguments
		lua_rawgeti(vm, LUA_REGISTRYINDEX, ref); // self
	}

	void ScriptingSlave::onWake(int ref, int ticks) {
		LuaStackguard guard(vm);

		prepareGObjectCall(ref, "onWake");

		lua_pushnumber(vm, ticks);	// ticks
		// make it a pcall ASAP
		if (lua_pcall(vm, 2, 0, 0)) {
			Log("scripting_slave") 
				<< "onWake called into invalid code: "
				<< lua_tostring(vm, -1);
			lua_pop(vm, 1);
		}
		lua_pop(vm, 1);
	}
	
	int ScriptingSlave::lua_registerObject(lua_State* L) {
		// Ugly, ugly fucker. static is not good. use lunar, tolua or similar to make calls to C++ better?
		lua_State* vm = m_instance->vm;
	
		int ref = luaL_ref(vm, LUA_REGISTRYINDEX);
		lua_pushnumber(vm, ref);
		return 1;
	}

	int ScriptingSlave::lua_execcommand(lua_State *L) {
		ScriptingSlave* me = m_instance;

		command::Info cmd;
		
		cmd.object    = Lunar<Object_LuaScript>::check(L, 1)->getObject();
		cmd.commandId = size_t(lua_tonumber(L, 2));

		// use checkinteger, checklong causes a "old-style cast"
		// warning as of LUA 5.1
		cmd.params[0] = luaL_checkinteger(L, 3);
		cmd.params[1] = luaL_checkinteger(L, 4);
		cmd.params[2] = luaL_checkinteger(L, 5);
		cmd.params[3] = luaL_checkinteger(L, 6);

		cmd.stringParam = std::string(luaL_checkstring(L, 7));

		me->sendEvent(makeEvent(FIFE_EXECCOMMAND, cmd));
		return 0;
	}
	
	int ScriptingSlave::lua_sleep(lua_State* L) {
		ScriptingSlave* me = m_instance;
	
		int ref    = int(lua_tonumber(L, 1));
		int ticks  = int(lua_tonumber(L, 2));

		if (me->m_sleeping_objects.find(ref) != me->m_sleeping_objects.end()) {
			Debug("scripting_slave")
				<< "lua table " << ref 
				<< " is already sleeping.";
			return 0;
		}
		me->m_sleeping_objects.insert(ref);
			
		sleeping_t s;

		s.id = ref;
		s.sleep_time = me->m_ticks;
		s.wake_time  = me->m_ticks + ticks;

		me->m_sleeping.push(s);
		return 0;
	}

	void ScriptingSlave::processEvent(const event_t& e) {
		LuaStackguard guard(vm);

		switch (e.code()) {
			case FIFE_HEARTBEAT: {
				m_ticks = e.ticks();
			} break;

			case FIFE_EXEC_STRING:
			case FIFE_EXEC_FILE:
			case FIFE_EXEC:
				processExecEvent(e);
			break;
			case FIFE_NEW_OBJECT:
				lua_getglobal(vm, "AddObject");
				Lunar<Object_LuaScript>::push(vm,
					new Object_LuaScript(e.get<ObjectPtr>()),true);

				if( lua_pcall(vm,1,0,0) ) {
					Warn("scripting_slave")
						<< "Coudn't execute script send by MapRunner: "
						<< lua_tostring(vm, -1);
					lua_pop(vm,1);
				}
			break;
	
			default: {
				AsyncBridge::processEvent(e);
			}
		}
	}

	void ScriptingSlave::processExecEvent(const event_t& e) {
		LuaStackguard guard(vm);

		const ScriptContainer& sc = e.get<ScriptContainer>();
		std::string script(sc.value);
		bool error_occured = false;

		if (sc.type == ScriptContainer::ScriptFile) {
			error_occured = 
				luaL_loadfile(vm, script.c_str()) ||
				lua_pcall(vm, 0, 0, 0); // not sure if I broke something here
		} else if (sc.type == ScriptContainer::ScriptString) {
			error_occured =
				luaL_loadbuffer(vm, script.c_str(), script.length(), "cmd") ||
				lua_pcall(vm, 0, 0, 0);
		}

		if( error_occured ) {
			Warn("scripting_slave")
				<< "Coudn't execute script send by MapRunner: "
				<< lua_tostring(vm, -1);
			lua_pop(vm,1);
		}
	}

	// XXX verify me
	int ScriptingSlave::wakeThem() {
		int n = 0;
		
		while (!m_sleeping.empty()) {
			sleeping_t s = m_sleeping.top();			
			int overslept = m_ticks - s.wake_time;
			if (overslept < 0) break;
			
			m_sleeping.pop();
			
			m_sleeping_objects.erase(s.id);

			onWake(s.id, overslept);
			n++;
		}
		return n;
	}
	
	int ScriptingSlave::loop() {
		while (!a_shutdown) {
			processEvents();
			wakeThem();
			sendEvents();
			SDL_Delay(1);
		}
		return m_ticks; // Why not? ;-)
	}

} }
