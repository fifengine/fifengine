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

#ifndef FIFE_MAP_SCRIPTINGSLAVE_H
#define FIFE_MAP_SCRIPTINGSLAVE_H

// Standard C++ library includes
#include <set>
#include <string>

// 3rd party library includes
// load Lua 5.1 libs
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "asyncbridge.h"

namespace FIFE { namespace map {

	class ScriptingSlave : public AsyncBridge {
		public:
			ScriptingSlave();
			virtual ~ScriptingSlave();
	
			int loop();
			/* Methods for saving/loading vm-state also go here */

		protected:
			lua_State* vm;

			// TODO: timewrap!
			long m_ticks;

			struct sleeping_t {
				int id;
				int sleep_time;
				int wake_time;
			};
			
			struct lessWakeTime {
				bool operator()(const sleeping_t& a, const sleeping_t& b) {
					// lowest wake_time should be at front
					return a.wake_time > b.wake_time; 
				}
			};

			typedef std::priority_queue<sleeping_t, std::vector<sleeping_t>, lessWakeTime> 	type_sleeping_queue;
			type_sleeping_queue m_sleeping;
	
			// Set of objects currently tracked:
			std::set<int> m_sleeping_objects;

			/** Dispatch an event from the ScriptingHost inside the FIFE-
			* engine to LUA-objects inside the VM. This will be 
			* notifications, path-calculations etc. as well as user-input 
			* via. keyboard/mouse.
			*/
			void processEvent(const event_t& e);

			void processExecEvent(const event_t& e);

			int wakeThem();

			/* These functions call the respective member-functions
			   in the lua-class identified by id */
			void onWake(int idx, int ticks);

			void prepareGObjectCall(int idx, const std::string& function);
	
			/* These are the primitive functions callable from LUA */
			static int lua_registerObject(lua_State* L);
			static int lua_sleep(lua_State* L);
			static int lua_execcommand(lua_State* L);

			struct s_slave {
				ScriptingSlave *slave;
			};
			s_slave* m_slave_wrapper;

			static ScriptingSlave* lua_getslave(lua_State* L);
	};
} } // FIFE::map

#endif

