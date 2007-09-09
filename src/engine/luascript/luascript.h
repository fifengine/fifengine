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

#ifndef LUASCRIPT_H
#define LUASCRIPT_H

/* lua 5.0 & luabind backend */
#include <string>
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// implements this
#include "scriptbase.h"

// using these
#include "debugutils.h"

// to access the following
#include "mapview.h"
//#include "gamestatemanager.h"

namespace FIFE
{
	class LuaScript : public ScriptEngine {
		public:
			LuaScript();
			virtual ~LuaScript();
			void start(const std::string scriptFile);
			void stop();
			void runFile(const std::string scriptFile);
			void runString(const char *string);
			void callFunction(const char *name);

			void setGlobalInt(const std::string name, const int val);
			void setGlobalDouble(const std::string name, const double val);
			void setGlobalString(const std::string name, const std::string val);
			int getGlobalInt(const std::string name);
			double getGlobalDouble(const std::string name);
			const char* getGlobalString(const std::string name);

			void registerMapViewInstance(MapView* currentMap);
			void setActiveMapElevation(size_t);
			static int set_next_mapfile(lua_State *L);
		private:
			//SINGLEFRIEND(LuaScript);
			lua_State *L;
			void _initLua();

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
