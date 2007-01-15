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
#include <string>

// 3rd party library includes

// FIFE includes
#include "exception.h"

#include "video/gui/guimanager.h"
#include "vfs/vfs.h"
#include "vfs/vfssource.h"
#include "vfs/vfssourcefactory.h"
#include "vfs/raw/rawdata.h"

#include "lua_vfs.h"

namespace FIFE {


	int VFS_LuaScript::read(lua_State *L) {
		std::string fname = luaL_checkstring(L,1);
		RawDataPtr data;

		try {
			data = VFS::instance()->open(fname);
		} catch( Exception& e) {
			Warn("lua_vfs")
				<< "Exception while trying to read: " << fname
				<< " (" << e.getMessage() << ")";
			lua_pushnil(L);
			return 1;
		}

		std::string retval( data->readString(data->getDataLength()) );
		lua_pushlstring(L,retval.c_str(),retval.size());
		return 1;
	}
	int VFS_LuaScript::readlines(lua_State *L) {
		std::string fname = luaL_checkstring(L,1);
		RawDataPtr data;

		try {
			data = VFS::instance()->open(fname);
		} catch( Exception& e) {
			Warn("lua_vfs")
				<< "Exception while trying to read: " << fname
				<< " (" << e.getMessage() << ")";
			lua_pushnil(L);
			return 1;
		}

		lua_newtable(L);

		std::string line;
		int n = 0;
		while(data->getLine(line)) {
			lua_pushlstring(L,line.c_str(),line.size());
			lua_rawseti(L,-2,n);
			++n;
		}
		return 1;
	}

	int VFS_LuaScript::addSource(lua_State *L) {
		std::string sname = luaL_checkstring(L, 1);
		VFSSource * src = VFSSourceFactory::instance()->createSource(sname);
		if (src)
			VFS::instance()->addSource(src);
		else
			Warn("lua_vfs") << "Invalid source: " << sname;
		return 0;
	}

	int luaopen_vfs(lua_State *L) {
		luaL_openlib(L, "vfs", VFS_LuaScript::methods, 0);
		return 0;
	}

#define method(name) {#name, VFS_LuaScript::name}

	const luaL_reg VFS_LuaScript::methods[] = {
		method(read),
		method(readlines),
		method(addSource),
		{NULL, NULL}
	};
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
