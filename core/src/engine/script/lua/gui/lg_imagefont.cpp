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
#include "guichan_addon/guichanfont.h"
#include "util/exception.h"
#include "util/log.h"

#include "luagui.h"

namespace luaGui {

	ImageFont::ImageFont(lua_State *L) {

		m_font = new FIFE::GuichanImageFont(
			luaL_checkstring(L, 1),
			luaL_checkstring(L, 2));

		if( m_font == 0 ) {
			FIFE::Warn("luaGui::ImageFont")
				<< "Couldn't load font " << lua_tostring(L, 1);
		}
	}
	ImageFont::~ImageFont() {
		delete m_font;
	}

	const char ImageFont::className[] = "ImageFont";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<ImageFont>::RegType ImageFont::methods[] = {
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
