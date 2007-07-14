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

#include "lua_widgetlistener.h"

namespace FIFE {


	LuaWidgetListener::LuaWidgetListener(lua_State* L) : LuaEventListener(L), IWidgetListener() {
		addKnownHandler(L,"onWidgetAction");
	}

	LuaWidgetListener::~LuaWidgetListener() {
	}

	void LuaWidgetListener::doReceiveEvents() {
		EventManager::instance()->addWidgetListener(this);
	}

	void LuaWidgetListener::doIgnoreEvents() {
		EventManager::instance()->removeWidgetListener(this);
	}

	lua_State* LuaWidgetListener::buildWidgetEvent(const IWidgetEvent& event) {
		lua_State* L = buildEvent(event);
		
		lua_pushstring(L,"action");
		lua_pushstring(L,event.getId().c_str());
		lua_settable(L,-3);
		
		return L;
	}

	void LuaWidgetListener::onWidgetAction(IWidgetEvent& evt) {
		lua_State* L = buildWidgetEvent(evt);
		dispatch(L,evt,"onWidgetAction");
	}

	const char LuaWidgetListener::className[] = "_WidgetListener";

#define method(class, name) {#name, &class::name}
	Lunar<LuaWidgetListener>::RegType LuaWidgetListener::methods[] = {
		method(LuaEventListener, getHandler),
		method(LuaEventListener, setHandler),
		method(LuaEventListener, receiveEvents),
		method(LuaEventListener, ignoreEvents),
		{0,0}
	};

}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
