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
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "script/lua.hpp"
#include "script/lunar.h"
#include "script/lua_ref.h"
#include "eventchannel/base/ec_ievent.h"

namespace FIFE {

	/** Lua Eventlistener Base class.
	 */
	class LuaEventListener{
		public:
			LuaEventListener(lua_State *L);
			virtual ~LuaEventListener();

			int setHandler(lua_State *L);
			int getHandler(lua_State *L);
			
			int receiveEvents(lua_State *L);
			int ignoreEvents(lua_State *L);

		protected:
			virtual void doReceiveEvents() = 0;
			virtual void doIgnoreEvents() = 0;
		
			lua_State* buildEvent(const IEvent& event);
			void dispatch(lua_State* L, IEvent& event, const std::string& eventname);
			void addKnownHandler(lua_State*L, const char* name);
			const char* checkHandler(lua_State* L, int index);
		private:
			LuaRef m_eventhandlers;
			LuaRef m_knownhandlers;
			bool m_listening;
	};

}
#endif
