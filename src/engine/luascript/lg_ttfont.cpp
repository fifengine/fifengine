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

#include "luagui.h"
namespace luaGui {
	TTFont::TTFont(lua_State *L) : 
		gcn::SDLTrueTypeFont(lua_tostring(L, 1), int(lua_tonumber(L, 2))) {
	}
	int TTFont::l_setColor(lua_State *L) {
		Uint8 r, g, b;
		r = Uint8(lua_tonumber(L, 1));
		g = Uint8(lua_tonumber(L, 2));
		b = Uint8(lua_tonumber(L, 3));
		gcn::SDLTrueTypeFont::setColor(r, g, b);
		return 0;
	}
	const char TTFont::className[] = "TTFont";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<TTFont>::RegType TTFont::methods[] = {
		method(TTFont, setColor),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
