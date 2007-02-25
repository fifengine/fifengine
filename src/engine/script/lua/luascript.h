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

#ifndef FIFE_SCRIPT_LUA_LUASCRIPT_H
#define FIFE_SCRIPT_LUA_LUASCRIPT_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include "lua.hpp"

// FIFE includes
// to access the following
#include "map/view.h"

// implements this
#include "script/scriptengine.h"

// using these
#include "debugutils.h"

namespace FIFE {

	class LuaScript : public ScriptEngine {
		public:
			LuaScript();
			virtual ~LuaScript();

			virtual void init();
			virtual void deinit();
			
			virtual void runFile(const std::string& scriptFile);
			virtual void runString(const std::string& string);
			virtual void callFunction(const std::string& name);

			virtual void setGlobalInt(const std::string& name, const int val);
			virtual void setGlobalDouble(const std::string& name, const double val);
			virtual void setGlobalString(const std::string& name, const std::string& val);
			virtual int getGlobalInt(const std::string& name) const;
			virtual double getGlobalDouble(const std::string& name) const;
			virtual std::string getGlobalString(const std::string& name) const;

			static int set_next_mapfile(lua_State *L);

			lua_State* getState() { return L; }
		private:
			//SINGLEFRIEND(LuaScript);
			lua_State *L;
			void _initLua();

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
