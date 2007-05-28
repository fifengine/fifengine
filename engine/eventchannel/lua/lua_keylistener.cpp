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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "eventchannel/manager/eventmanager.h"

#include "lua_keylistener.h"

namespace FIFE {


	LuaKeyListener::LuaKeyListener(lua_State* L) : LuaEventListener(L), IKeyListener() {
		addKnownHandler(L, "keyPressed" );
		addKnownHandler(L, "keyReleased" );
	}

	LuaKeyListener::~LuaKeyListener() {
	}

	void LuaKeyListener::doReceiveEvents() {
		EventManager::instance()->addKeyListener(this);
	}

	void LuaKeyListener::doIgnoreEvents() {
		EventManager::instance()->removeKeyListener(this);
	}

	lua_State* LuaKeyListener::buildKeyEvent(const IKeyEvent& event) {
		lua_State* L = buildInputEvent(event);
		
		lua_pushstring(L,"key");
		lua_newtable(L);
		
		lua_pushstring(L,"ischaracter");
		lua_pushboolean(L,event.getKey().isCharacter());
		lua_settable(L,-3);
		
		lua_pushstring(L,"isnumber");
		lua_pushboolean(L,event.getKey().isNumber());
		lua_settable(L,-3);
		
		lua_pushstring(L,"isletter");
		lua_pushboolean(L,event.getKey().isLetter());
		lua_settable(L,-3);
		
		lua_pushstring(L,"value");
		lua_pushinteger(L,event.getKey().getValue());
		lua_settable(L,-3);
		
		lua_pushstring(L,"string");
		lua_pushstring(L,event.getKey().getAsString().c_str());
		lua_settable(L,-3);
		
		lua_settable(L,-3);

		return L;
	}

	void LuaKeyListener::keyPressed(IKeyEvent& evt) {
		lua_State* L = buildKeyEvent(evt);
		dispatch(L,evt,"keyPressed");
	}

	void LuaKeyListener::keyReleased(IKeyEvent& evt) {
		lua_State* L = buildKeyEvent(evt);
		dispatch(L,evt,"keyReleased");
	}

	const char LuaKeyListener::className[] = "KeyListener";

#define method(class, name) {#name, &class::name}
	Lunar<LuaKeyListener>::RegType LuaKeyListener::methods[] = {
		method(LuaEventListener, getHandler),
		method(LuaEventListener, setHandler),
		method(LuaEventListener, receiveEvents),
		method(LuaEventListener, ignoreEvents),
		{0,0}
	};

}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
