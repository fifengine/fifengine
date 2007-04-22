/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include <cassert>
#include <string.h>
#include <list>
#include "luascript.h"
#include "lua_mapview.h"
#include "lua_object.h"
// #include "lua_log.h"
#include "mapelevation.h"
#include "object.h"
#include "lunar.h"
#include "lua_gui_actionlistener.h"
#include "mapviewgamestate.h"
#include "gamestatemanager.h"
#include "lua_stackguard.h"

namespace FIFE {
	extern int luaopen_mapview(lua_State* L);
// 	extern int luaopen_console(lua_State* L);
// 	extern int luaopen_log(lua_State* L);
	extern int luaopen_audiomanager(lua_State *L);
	extern int luaopen_inputmanager(lua_State *L);
	extern int luaopen_guimanager(lua_State *L);
	extern int luaopen_gsmanager(lua_State *L);

	void LuaScript::_initLua() {
		L = lua_open();
		if (L == NULL)
			throw FIFE::ScriptException(implementationName +
					" - Creating LUA state failed");
		implementationName = LUA_VERSION;

		//FIXME: hardcoded
		luaopen_base(L);
		luaopen_io(L);
		luaopen_string(L);
		luaopen_math(L);

		lua_pushcfunction(L, set_next_mapfile);
		lua_setglobal(L, "set_next_mapfile");

		luaopen_gsmanager(L);
// 		luaopen_log(L);
		luaopen_mapview(L);
// 		luaopen_console(L);
		luaopen_audiomanager(L);
		luaopen_inputmanager(L);
		luaopen_guimanager(L);
		Lunar<Object_LuaScript>::Register2(L);
	}

	LuaScript::LuaScript() {
		new ActionListener_Lua();
		Log("LuaScript") << "Scripting enabled: " << implementationName;
		L = NULL;
	}

	void LuaScript::start(const std::string scriptFile) {
		if (L != NULL) {
			Log("LuaScript") << "state already started? Stopping it first!";
			stop();
		}
		_initLua();
		runFile(scriptFile);
	}

	void LuaScript::stop() {
		if (L != NULL) {
			lua_close(L);
			L = NULL;
			Log("LuaScript") << "scripting disabled";
		}
	}

	LuaScript::~LuaScript() {
		stop(); // just in case
	}

	/* now the base calls from c++ to the script layer ... */

	void LuaScript::runFile(const std::string filename) {
		LuaStackguard guard(L);
		if (filename.size() > 0) {
			Log("LuaScript") << std::string("loading file: ") << filename;
			if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
				throw FIFE::ScriptException(lua_tostring(L,-1));
		}
	}

	void LuaScript::runString(const char *s) {
		LuaStackguard guard(L);
		if (*s != 0) {
			Log("LuaScript") << std::string("loading string:") << std::string(s);
			if (luaL_loadbuffer(L, s, strlen(s), "cmd") || lua_pcall(L, 0, 0, 0))
				throw FIFE::ScriptException(lua_tostring(L,-1));
		}
	}

	int lua_follow_key(lua_State *L, const char* key) {
		char buffer[200];
		strncpy(reinterpret_cast<char*>(&buffer), key, 199);
		char *start = reinterpret_cast<char*>(&buffer);
		char *end = strstr(start, ".");
		while (end != NULL) {
			*end = '\0';
			lua_pushstring(L, start);
			//printf("Debug: search sub-key: %s\n", start);
			lua_gettable(L, -2);
			if (!lua_istable(L, -1)) {
				throw FIFE::ScriptException(std::string("Unable to find key element ") + 
						start + std::string(" from query ") + key);
			}
			start = end + 1;
			end = strstr(start, ".");
		}
		if (end == NULL) {
			lua_pushstring(L, start);
		}
		lua_gettable(L, -2);
		return 0;
	}

	void LuaScript::callFunction(const char *name) {
		char* sep = strstr(name, ".");
		LuaStackguard guard(L);

		if (sep == NULL)
			lua_getglobal(L, name);
		else {
			char buffer[200];
			strncpy(reinterpret_cast<char*>(&buffer), name, 199);
			sep = strstr(buffer, ".");
			*sep = '\0';
			lua_getglobal(L, buffer);
			if (lua_type(L, -1) != LUA_TTABLE) {
				lua_pop(L, 1);
				throw FIFE::ScriptException(std::string("Cannot call ") + name + std::string(" as ") + 
						buffer + std::string(" is not a table"));
			}
			lua_follow_key(L, ++sep);
		}
		if (lua_type(L, -1) == LUA_TFUNCTION) {
			if (lua_pcall(L, 0, 0, 0) != 0)
				throw FIFE::ScriptException(std::string("Error running: ") + std::string(name) +
						lua_tostring(L, -1));
		}
		else
			Log("LuaScript") << "Ignoring call to " << name << " - no such lua function";
	}

	void LuaScript::setGlobalInt(const std::string name, const int val) {
		LuaStackguard guard(L);
		lua_pushnumber(L, val);
		lua_setglobal(L, name.c_str());
	}

	void LuaScript::setGlobalDouble(const std::string name, const double val) {
		LuaStackguard guard(L);
		lua_pushnumber(L, val);
		lua_setglobal(L, name.c_str());
	}

	void LuaScript::setGlobalString(const std::string name, const std::string val) {
		LuaStackguard guard(L);
		lua_pushstring(L, val.c_str());
		lua_setglobal(L, name.c_str());
	}

	int LuaScript::getGlobalInt(const std::string name) {
		LuaStackguard guard(L);
		int result = 0;
		lua_getglobal(L, name.c_str());
		if (!lua_isnumber(L, -1))
			throw FIFE::ScriptException("INT value expected in getGlobalInt("+name+")");
		result = int(lua_tonumber(L, -1));
		lua_pop(L, 1);
		return result;
	}

	double LuaScript::getGlobalDouble(const std::string name) {
		LuaStackguard guard(L);
		double result = 0.0f;
		lua_getglobal(L, name.c_str());
		if (!lua_isnumber(L, -1))
			throw FIFE::ScriptException("DOUBLE value expected in getGlobalDouble("+name+")");
		result = lua_tonumber(L, -1);
		lua_pop(L, 1);
		return result;
	}

	const char* LuaScript::getGlobalString(const std::string name) {
		LuaStackguard guard(L);
		const char* result = NULL;
		lua_getglobal(L, name.c_str());
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException("STRING value expected in getGlobalString("+name+")");
		result = lua_tostring(L, -1);
		lua_pop(L, 1);
		return result;

	}

	void LuaScript::registerMapViewInstance(MapView* currentMap) {
		MapView_LuaScript::registerMap(currentMap);

	}

	void LuaScript::setActiveMapElevation(size_t id) {
		LuaStackguard guard(L);
		MapView* mv = MapView_LuaScript::getMapView();
		assert(mv!=NULL);
		lua_getglobal(L, "map");
		if (lua_istable(L, -1)) {
			Log("luascript") << "Info: already have loaded map - deleting";
			lua_pop(L, 1);
			lua_pushnil(L);
			lua_setglobal(L, "map");
			// run gc ?
		}
		lua_newtable(L);
		lua_setglobal(L, "map");
		// make sure it is on top
		lua_getglobal(L, "map");

		MapElevation * me = mv->getCurrentElevation();
		assert(me!=NULL);
		std::list<Object*> obj_list = me->getObjects();
		std::list<Object*>::iterator i = obj_list.begin();
		int c = 0;
		while (i != obj_list.end()) {
			//lua_pushlightuserdata(L, static_cast<void*>(*i));
			Object_LuaScript * lo = new Object_LuaScript(*i);
			lua_pushnumber(L, c);
			//lua_pushlightuserdata(L, static_cast<void*>(lo));
			Lunar<Object_LuaScript>::push(L, lo, true);
			lua_settable(L, -3);
			i++;
			c++;
		}
		// pop 'map' of the stack
		lua_pop(L, 1);
	}

	int LuaScript::set_next_mapfile(lua_State *L) {
		if (!lua_isstring(L, -1))
			throw FIFE::ScriptException(std::string("String value expected:") + __PRETTY_FUNCTION__);
		MapViewGameState *mvgs = NULL;
		GameStateManager *gsm = GameStateManager::instance();
		assert(gsm);
		if (gsm->gameStateExists("MapView")) { // not good; hardcoded value!
			mvgs = static_cast<MapViewGameState*>(gsm->getGameState("MapView"));
			mvgs->setMapFile(lua_tostring(L, -1));
		}
		else
			throw FIFE::ScriptException("Error: no MapView GameState registered!");
		return 0;
	}

	/* now the calls from the scripts to c++ */

	/*void LuaScript::activateGameState(const std::string name) {
		if (gsm->gameStateExists(name))
		gsm->activate(name);
		}*/

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
