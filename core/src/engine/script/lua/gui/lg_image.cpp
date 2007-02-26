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
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/scriptbackendmanager.h"

#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "video/gui/gcnfifeimage.h"
#endif

#include "video/animation.h"
#include "debugutils.h"

#include "luagui.h"

namespace luaGui {

	Image::Image(lua_State *L) : gcn::AdvImage(lua_tostring(L, 1)) {
	}
	int Image::l_getWidth(lua_State *L) {
#if GUICHAN_VERSION == 4
		lua_pushnumber(L, gcn::Image::getWidth());
#else
		lua_pushnumber(L, FIFE::GCNImage::getWidth());
#endif
		return 1;
	}
	int Image::l_getHeight(lua_State *L) {
#if GUICHAN_VERSION == 4
		lua_pushnumber(L, gcn::Image::getHeight());
#else
		lua_pushnumber(L, FIFE::GCNImage::getHeight());
#endif
		return 1;
	}
	int Image::l_setAnimActive(lua_State *L) {
		setAnimActive(lua_toboolean(L, 1));
		return 0;
	}
	int Image::l_isAnimation(lua_State *L) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a) {
			lua_pushboolean(L,0);
		} else {
			lua_pushboolean(L,1);
		}
		return 1;
	}
	int Image::l_setAnimDirection(lua_State *L) {
		setAnimDirection(lua_toboolean(L, 1));
		return 0;
	}
	int Image::l_setAnimEndCallback(lua_State *L) {
		FIFE::Animation* a = getImageAsAnimation();
		if (!a)
			return 0;
		if (lua_isnil(L, 1))
			a->setOnEndCallback(NULL);
		else
			a->setOnEndCallback(boost::bind(&Image::animEndCallback, this));
		m_callbackName = "gui.anim_end.";
		m_callbackName.append(luaL_checkstring(L, 1));
		return 0;
	}
	void Image::animEndCallback() {
		FIFE::CScriptEngine()->callFunction(m_callbackName.c_str());
	}
	const char Image::className[] = "Image";
#define method(class, name) {#name, &class::l_ ## name}

	Lunar<Image>::RegType Image::methods[] = {
		method(Image, getWidth),
		method(Image, getHeight),
		method(Image, setAnimActive),
		method(Image, setAnimDirection),
		method(Image, setAnimEndCallback),
		method(Image, isAnimation),
		{0,0}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
