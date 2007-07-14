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

#include "lua_mouselistener.h"

namespace FIFE {


	LuaMouseListener::LuaMouseListener(lua_State* L) : LuaEventListener(L), IMouseListener() {
		addKnownHandler(L, "mouseEntered" );
		addKnownHandler(L, "mouseExited" );
		addKnownHandler(L, "mousePressed" );
		addKnownHandler(L, "mouseReleased" );
		addKnownHandler(L, "mouseClicked" );
		addKnownHandler(L, "mouseWheelMovedUp" );
		addKnownHandler(L, "mouseWheelMovedDown" );
		addKnownHandler(L, "mouseDragged" );
		addKnownHandler(L, "mouseMoved" );
	}

	LuaMouseListener::~LuaMouseListener() {
	}

	void LuaMouseListener::doReceiveEvents() {
		EventManager::instance()->addMouseListener(this);
	}

	void LuaMouseListener::doIgnoreEvents() {
		EventManager::instance()->removeMouseListener(this);
	}

	lua_State* LuaMouseListener::buildMouseEvent(const IMouseEvent& event) {
		lua_State* L = buildInputEvent(event);
		
		lua_pushstring(L,"x");
		lua_pushinteger(L,event.getX());
		lua_settable(L,-3);
		
		lua_pushstring(L,"y");
		lua_pushinteger(L,event.getY());
		lua_settable(L,-3);
		
		lua_pushstring(L,"eventType");
		lua_pushstring(L,mouseEventType2str(event.getType()).c_str());
		lua_settable(L,-3);
		
		lua_pushstring(L,"button");
		lua_pushstring(L,mouseButtonType2str(event.getButton()).c_str());
		lua_settable(L,-3);
		
		return L;
  	}

	void LuaMouseListener::mouseEntered(IMouseEvent& evt) {
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseEntered");
	}

	void LuaMouseListener::mouseExited(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseExited");
	}


	void LuaMouseListener::mousePressed(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mousePressed");
	}


	void LuaMouseListener::mouseReleased(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseReleased");
	}


	void LuaMouseListener::mouseClicked(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseClicked");
	}


	void LuaMouseListener::mouseWheelMovedUp(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseWheelMovedUp");
	}


	void LuaMouseListener::mouseWheelMovedDown(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseWheelMovedDown");
	}


	void LuaMouseListener::mouseMoved(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseMoved");
	}


	void LuaMouseListener::mouseDragged(IMouseEvent& evt){
		lua_State *L = buildMouseEvent(evt);
		dispatch(L, evt, "mouseDragged");
	}

	const char LuaMouseListener::className[] = "MouseListener";

#define method(class, name) {#name, &class::name}
	Lunar<LuaMouseListener>::RegType LuaMouseListener::methods[] = {
		method(LuaEventListener, getHandler),
		method(LuaEventListener, setHandler),
		method(LuaEventListener, receiveEvents),
		method(LuaEventListener, ignoreEvents),
		{0,0}
	};

}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
