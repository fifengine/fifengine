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
#include "guichan_addon/aafont.h"
#include "util/exception.h"
#include "util/log.h"

#include "luagui.h"

namespace luaGui {

	AAFont::AAFont(lua_State *L) {
		m_font = new FIFE::AAImageFont(luaL_checkstring(L, 1));

		if( m_font == 0 ) {
			FIFE::Warn("luaGui::AAFont")
				<< "Couldn't load font " << lua_tostring(L, 1);
		}
	}
	AAFont::~AAFont() {
		delete m_font;
	}

	int AAFont::l_setColor(lua_State *L) {
		if( m_font == 0 )
			return 0;
		
		Uint8 r, g, b;
		gcn::Color * c = NULL;

		switch (lua_gettop(L)) {
			case 3:
				r = Uint8(lua_tonumber(L, 1));
				g = Uint8(lua_tonumber(L, 2));
				b = Uint8(lua_tonumber(L, 3));
				m_font->setColor(r, g, b);
				break;
			case 1:
				c = AWidget::lua2gcn_color_cast(L);
				if (c)
					m_font->setColor(c->r, c->g, c->b);
				break;
			default:
				FIFE::Log("AAFont::setColor") << "Invalid parameter";
				break;
		}
		return 0;
	}
	const char AAFont::className[] = "AAFont";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<AAFont>::RegType AAFont::methods[] = {
		method(AAFont, setColor),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
