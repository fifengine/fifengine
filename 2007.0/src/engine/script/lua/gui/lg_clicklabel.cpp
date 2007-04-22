/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "log.h"

#include "luagui.h"

namespace luaGui {
	
	ClickLabel::ClickLabel(lua_State *L) : gcn::Button(luaL_checkstring(L,1)) {
		setBorderSize(0);
		adjustSize();
	}
	ClickLabel::~ClickLabel() {}
	int ClickLabel::l_getAlignment(lua_State *L) {
		lua_pushnumber(L, gcn::Button::getAlignment());
		return 1;
	}
	int ClickLabel::l_setAlignment(lua_State *L) {
		int v = int(luaL_checknumber(L, 1));
		gcn::Button::setAlignment(v);
		return 0;
	}
	int ClickLabel::l_getCaption(lua_State *L) {
		const std::string & sr = gcn::Button::getCaption();
		lua_pushstring(L, sr.c_str());
		return 1;
	}
	int ClickLabel::l_setCaption(lua_State *L) {
		const char* v = luaL_checkstring(L, 1);
		gcn::Button::setCaption(v);
		adjustSize();
		return 0;
	}
	void ClickLabel::draw(gcn::Graphics* graphics) {
		int textX;
		int textY = getHeight() / 2 - getFont()->getHeight() / 2;

		switch (getAlignment())
		{
			case gcn::Graphics::LEFT:
				textX = 0;
				break;
			case gcn::Graphics::CENTER:
				textX = getWidth() / 2;
				break;
			case gcn::Graphics::RIGHT:
				textX = getWidth();
				break;
			default:
				throw GCN_EXCEPTION("Unknown alignment.");
		}

		graphics->setFont(getFont());
		graphics->setColor(getForegroundColor());
		graphics->drawText(getCaption(), textX, textY, getAlignment());
	}

	const char ClickLabel::className[] = "ClickLabel";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<ClickLabel>::RegType ClickLabel::methods[] = {
		method(ClickLabel, getAlignment),
		method(ClickLabel, setAlignment),
		method(ClickLabel, getCaption),
		method(ClickLabel, setCaption),
		method(ClickLabel, setFont),

		LUAGUI_WIDGET_DEF(ClickLabel),

		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
