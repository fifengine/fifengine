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

#ifndef LUA_VFS_H
#define LUA_VFS_H

// Standard C++ library includes

// 3rd party library includes
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// FIFE includes

namespace FIFE {

	class VFS_LuaScript {
		public:

			static int read(lua_State *L);
			static int readlines(lua_State *L);

			static int listFiles(lua_State *L);
			static int listDirectories(lua_State *L);

			static int addSource(lua_State *L);

			static const luaL_reg methods[];
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
