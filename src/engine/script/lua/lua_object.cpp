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

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "lua_object.h"

namespace FIFE {


	Object_LuaScript::Object_LuaScript(map::ObjectInfo *obj) {
		//m_object = static_cast<Object*>(lua_touserdata(L, 1));
		m_object = obj;
	}
	Object_LuaScript::~Object_LuaScript() {
	}

	const char Object_LuaScript::className[] = "GObject";

#define method(class, name) {#name, &class::name}
	Lunar<Object_LuaScript>::RegType Object_LuaScript::methods[] = {
/*		method(Object_LuaScript, getPosition),
		method(Object_LuaScript, setPosition),
		method(Object_LuaScript, getTypeName),*/
		{0,0}
	};

}

/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
