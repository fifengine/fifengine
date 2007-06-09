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

#ifndef FIFE_SCRIPT_LUA_LUA_MAPCAMERA_H
#define FIFE_SCRIPT_LUA_LUA_MAPCAMERA_H

// Standard C++ library includes

// 3rd party library includes
#include "lua.hpp"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "script/lunar.h"
#include "map/camera.h"

namespace FIFE {

	class MapCamera_Lunar : public map::Camera
	{
		public:
			static const char* className;
			static Lunar<MapCamera_Lunar>::RegType methods[];

			MapCamera_Lunar(lua_State *L);
			~MapCamera_Lunar();

			int l_render(lua_State *L);
			int l_setViewport(lua_State *L);

			int l_moveBy(lua_State *L);
			int l_jumpTo(lua_State *L);

			int l_moveTo(lua_State *L);
			int l_track(lua_State *L);

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
