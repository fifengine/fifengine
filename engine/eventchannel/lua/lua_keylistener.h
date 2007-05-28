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

#ifndef FIFE_EVENTCHANNEL_LUA_KEY_LISTENER_H
#define FIFE_EVENTCHANNEL_LUA_KEY_LISTENER_H

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
#include "eventchannel/key/ec_ikeyevent.h"
#include "eventchannel/key/ec_ikeylistener.h"

#include "lua_listener.h"

namespace FIFE {

	/** Lua Key Listener
	 */
	class LuaKeyListener : public LuaEventListener, public IKeyListener {
		public:
			static const char className[];
			static Lunar<LuaKeyListener>::RegType methods[];

			LuaKeyListener(lua_State *L);
			virtual ~LuaKeyListener();
			
			/** Called if a key is pressed
			 * If a key is held down the multiple key presses are generated.
			 * @param keyevent discribes the event.
			 */
			virtual void keyPressed(IKeyEvent& evt);
	
			/** Called if a key is released
			 * @param keyevent discribes the event.
			 */
			virtual void keyReleased(IKeyEvent& evt);

		protected:
			virtual void doReceiveEvents();
			virtual void doIgnoreEvents();
		
			lua_State* buildKeyEvent(const IKeyEvent& event);
		
	};

}
#endif
