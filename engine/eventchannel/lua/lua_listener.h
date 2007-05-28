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

#ifndef FIFE_EVENTCHANNEL_LUA_LISTENER_H
#define FIFE_EVENTCHANNEL_LUA_LISTENER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "script/lua.hpp"
#include "script/lunar.h"
#include "script/lua_ref.h"
#include "eventchannel/base/ec_ievent.h"
#include "eventchannel/base/ec_iinputevent.h"

namespace FIFE {

	/** Lua Eventlistener Base class.
	 *  This class provides the basic lua wrapping infrastructure
	 *  for any derived listeners.
	 *
	 *  Basically you have to derive from this and a IXYZListener.
	 *  Then you need to override the doXYZEvents functions, so that
	 *  receiveEvents/ignoreEvents work as expected.
	 *
	 *  Then in your constructor call addKnownHandler with the names
	 *  of the callbacks you want to provide.
	 *
	 *  Then override the IXYZListener callback functions (like mouseClicked)
	 *  and implement them as follows:
	 *  @code
	 *  void xyzHappened(IXYZEvent& event) {
	 *     lua_State *L = buildEvent(event);
	 *     // manipulate the event table on the top of the stack.
	 *     dispatch(L, event, "xyzHappened");
	 *  }
	 *  @endcode
	 *
	 *  @note This class is designed to be created only from Lua.
	 *  @note Don't forget to register subclasses in script/luascript.cpp
	 */
	class LuaEventListener{
		public:
			/** Constructor
			 */
			LuaEventListener(lua_State *L);
			
			/** Destructor
			 *  Calls ignoreEvents so that no stale listener is registered anywhere.
			 */
			virtual ~LuaEventListener();

			/** Lua callback to set handler
			 * Expects a known handler name and a lua function with the
			 * signature function(event) -> nil
			 * You may set a handler to nil to disable it.
			 */
			int setHandler(lua_State *L);
			
			/** Get the Lua callback handler
			 */
			int getHandler(lua_State *L);
			
			/** Start receiving events
			 */
			int receiveEvents(lua_State *L);
			
			/** Stop receiving events
			 */
			int ignoreEvents(lua_State *L);

		protected:
			/// Override this to add self to the event controler
			virtual void doReceiveEvents() = 0;
			
			/// Override this to remove self from the event controler
			virtual void doIgnoreEvents() = 0;
		
			/** Build an event table from the incoming event
			 */
			lua_State* buildEvent(const IEvent& event);
			
			/** Build an event table from an incoming input event
			 */
			lua_State* buildInputEvent(const IInputEvent& event);

			/** Dispatch the build event table to the method eventname
			 *  Calls a handler set in lua which must have the name eventname
			 */
			void dispatch(lua_State* L, IEvent& event, const std::string& eventname);
			
			/** Add a known handler name
			 * Call this in your derived listener, to build the table
			 * of known handlers, which can be set from lua.
			 */
			void addKnownHandler(lua_State*L, const char* name);
			
			/** Check whether an argument is a known handler
			 *  Call this from lua to check whether a string at index
			 *  on the stack is a known handler. Calls luaL_argerror
			 *  otherwise.
			 */ 
			const char* checkHandler(lua_State* L, int index);

		private:
			/// Table of lua functions for events
			LuaRef m_eventhandlers;
			
			/// Set of known handler names
			LuaRef m_knownhandlers;
			
			/// Listening flag.
			bool m_listening;
			
			/// Event stack guard
			int m_eventStackTop;
	};

}
#endif
