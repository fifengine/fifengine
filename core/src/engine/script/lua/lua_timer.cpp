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

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log.h"

#include "lua_timer.h"

namespace FIFE {

	Timer_Lunar::Timer_Lunar(lua_State *L)
		: Timer() {
		Timer::setCallback( boost::bind( &Timer_Lunar::update , this ) );
	}

	Timer_Lunar::~Timer_Lunar() {
	}

	int Timer_Lunar::l_start(lua_State *L) {
		Timer::start();
		return 0;
	}

	int Timer_Lunar::l_stop(lua_State *L) {
		Timer::stop();
		return 0;
	}

	int Timer_Lunar::l_setInterval(lua_State *L) {
		int32_t msec = int32_t(luaL_checknumber(L,1));
		Timer::setInterval(msec);
		return 0;
	}

	int Timer_Lunar::l_setCallback(lua_State *L) {
		if( lua_gettop(L) != 1 || !lua_isfunction(L,1) ) {
			m_callback.unref();
			return 0;
		}

		m_callback.ref(L,1);
		return 0;
	}

	void Timer_Lunar::update() {
		lua_State* L = m_callback.getState();

		if( !m_callback.isValid() ) {
			Timer::stop();
			return;
		}

		m_callback.push();
		if( 0 != lua_pcall(L,0,0,0) ) {
			Warn("lua::Timer")
				<< "Error in callback " 
				<< lua_tostring(L,-1);
			m_callback.unref();
			Timer::stop();
		}
	}

	const char* Timer_Lunar::className = "Timer";

#define method(class, name) {#name, &class::l_  ## name}

	Lunar<Timer_Lunar>::RegType Timer_Lunar::methods[] = {
		method(Timer_Lunar, start),
		method(Timer_Lunar, stop),
		method(Timer_Lunar, setInterval),
		method(Timer_Lunar, setCallback),
		{0,0}
	};

}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
