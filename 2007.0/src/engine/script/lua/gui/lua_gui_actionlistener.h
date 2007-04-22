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

#ifndef LUA_GUI_ACTIONLISTENER
#define LUA_GUI_ACTIONLISTENER

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <guichan.hpp>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// FIFE includes
#include "singleton.h"

namespace FIFE {
	class ActionListener_Lua : public gcn::ActionListener,
				 public DynamicSingleton<ActionListener_Lua>
	{
		public:
			ActionListener_Lua();
			~ActionListener_Lua();
#ifdef GUICHAN05
			void action(const std::string&, gcn::Widget*);
#else
			void action(const std::string &action);
#endif
			void setup(lua_State *L, const std::string &global, const std::string &table);
		private:
			void luaAction(const std::string &action);
			std::string m_global;
			std::string m_table;
			lua_State *m_L;
			bool isReady;
	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
