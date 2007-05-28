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
#include <cassert>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/lua.hpp"
#include "eventchannel/manager/eventmanager.h"

#include "lua_listener.h"

namespace FIFE {

	static int consume(lua_State* L) {
		if( !lua_istable(L,1) ) {
			luaL_argerror(L,1,"event table expected");
			return 0;
		}
		lua_pushstring(L,"isconsumed");
		lua_pushvalue(L,-1);
		lua_gettable(L,-3);
		if( lua_isnil(L,-1) ) {
			luaL_argerror(L,1,"event table expected - 'isconsumed' missing");
			return 0;
		}
		lua_pop(L,1);
		lua_pushboolean(L,0);
		lua_settable(L,-3);
		return 0;
	}

	LuaEventListener::LuaEventListener(lua_State* L) 
		: m_eventhandlers(), m_knownhandlers(), m_listening(false) {
		lua_newtable(L);
		m_eventhandlers.ref(L,-1);
		
		lua_newtable(L);
		m_knownhandlers.ref(L,-1);

		lua_pop(L,2);
	}

	LuaEventListener::~LuaEventListener() {
		ignoreEvents(0);
	}

	int LuaEventListener::setHandler(lua_State* L) {
		(void)checkHandler(L,1);
		
		if( !lua_isfunction(L,2) && !lua_isnil(L,2) ) {
			luaL_argerror(L,2,"function or nil");
			return 0;
		}
		
		m_eventhandlers.push();
		lua_pushvalue(L,1);
		lua_pushvalue(L,2);
		lua_settable(L,3);
		//lua_pop(L,3);
		return 0;
	}
	
	int LuaEventListener::getHandler(lua_State* L) {
		(void)checkHandler(L,1);
		m_eventhandlers.push();
		lua_pushvalue(L,1);
		lua_gettable(L,2);
		return 1;
	}
	
	const char* LuaEventListener::checkHandler(lua_State* L, int index) {
		assert( L == m_knownhandlers.getState() );
		const char* handlername = luaL_checkstring(L,index);
		m_knownhandlers.push();
		lua_pushvalue(L,index);
		lua_gettable(L,-2);
		if( lua_isnil(L,-1) ) {
			luaL_argerror(L,index,"unknown handler");
			return 0; // never executed.
		}
		lua_pop(L,2);
		return handlername;
	}
	
	void LuaEventListener::addKnownHandler(lua_State*L, const char* name) {
		m_knownhandlers.push();
		lua_pushstring(L,name);
		lua_pushinteger(L,1);
		lua_settable(L,-3);
		lua_pop(L,1);
	}

	lua_State* LuaEventListener::buildEvent(const IEvent& event) {
		lua_State* L = m_knownhandlers.getState();
		m_eventStackTop = lua_gettop(L);
		lua_newtable(L);
		
		lua_pushstring(L,"name");
		lua_pushstring(L,event.getName().c_str());
		lua_settable(L,-3);
		
		lua_pushstring(L,"timestamp");
		lua_pushinteger(L,event.getTimeStamp());
		lua_settable(L,-3);
		
		lua_pushstring(L,"isconsumed");
		lua_pushboolean(L,event.isConsumed());
		lua_settable(L,-3);

		lua_pushstring(L,"consume");
		lua_pushcclosure(L, consume, 0);
		lua_settable(L,-3);
		
		return L;
	}
	
	lua_State* LuaEventListener::buildInputEvent(const IInputEvent& event) {
		lua_State* L = buildEvent(event);

		lua_pushstring(L,"mods");
		lua_newtable(L);
		
		lua_pushstring(L,"alt");
		lua_pushboolean(L,event.isAltPressed());
		lua_settable(L,-3);
		
		lua_pushstring(L,"control");
		lua_pushboolean(L,event.isControlPressed());
		lua_settable(L,-3);

		lua_pushstring(L,"meta");
		lua_pushboolean(L,event.isMetaPressed());
		lua_settable(L,-3);

		lua_pushstring(L,"shift");
		lua_pushboolean(L,event.isShiftPressed());
		lua_settable(L,-3);
		
		lua_settable(L,-3);

		return L;
	}
	
	void  LuaEventListener::dispatch(lua_State* L, IEvent& event, const std::string& eventname) {
		m_eventhandlers.push();
		lua_pushstring(L,eventname.c_str());
		lua_gettable(L,-2); 
		lua_remove(L,-2);
		
		if( lua_isnil(L,-1) ) {
			// no handler set
			lua_pop(L,2);
		} else {
			// stack layout: ... event, handler - reverse it and call.
			lua_insert(L,-2);
			LuaRef eventtable;
			eventtable.ref(L,-1);
			
			if( lua_pcall(L,1,0,0) ) {
				const char* error_message = lua_tostring(L,-1);
				Warn("LuaEventListener")
					<< "Error in event callback:" << error_message;
				lua_pop(L,1);
			} else {
				// check whether the event.isconsumed entry changed to "false"
				// and if that's the case - call consume 
				eventtable.push();
				lua_pushstring(L,"isconsumed");
				lua_gettable(L,-2);
				if( !lua_toboolean(L,-1) ) {
					event.consume();
				}
				lua_pop(L,2);
			}
		}
		
		if( m_eventStackTop != lua_gettop(L) ) {
			Warn("LuaEventListener")
				<< "Event stack unbalanced - is: " << lua_gettop(L)
				<< " should be:" << m_eventStackTop;
			lua_settop(L,m_eventStackTop);
		}
	}

	int LuaEventListener::receiveEvents(lua_State *L) {
		if( !m_listening ) {
			m_listening = true;
			doReceiveEvents();
		}
		return 0;
	}

	int LuaEventListener::ignoreEvents(lua_State *L) {
		if( m_listening ) {
			m_listening = false;
			doIgnoreEvents();
		}
		return 0;
	}

}
