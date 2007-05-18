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

#include "util/debugutils.h"
#include "util/singleton.h"

namespace FIFE {

	class ScriptContainer;
	class LuaScript: public DynamicSingleton<LuaScript> {
		public:
			LuaScript();
			virtual ~LuaScript();

			virtual void init();
			virtual void deinit();
			
			/** Run a scriptfile.
			 * @param filename VFS path to the script
			 *
			 * \note The VM state is not cleared when calling this; global variables
			 * will be shared between scripts.
			 *
			 * \note Any kind of exception caused by calling this script will
			 * be returned to the caller; this is valid for all \b run* functions.
			 *
			 * \see start function
			 */
			virtual void runFile(const std::string& scriptFile);

			/** Run a string.
			 * @param string with valid code (for the current backend)
			 *
			 * \note The quick-n-dirty way to run something.
			 */
			virtual void runString(const std::string& string);
			
			/** Call a script-function.
			 * @param Name of the function to call.
			 *
			 * \note Used for simple callbacks (the GUI); no arguments or return
			 * values, but a lot faster than calling \c runString().
			 */
			virtual void callFunction(const std::string& name);

			/** Execute a ScriptContainer
			 *
			 * @param script The ScriptContainer to run
			 */
			virtual void execute(const ScriptContainer& sc);

			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalInt(const std::string& name, const int val);

			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalDouble(const std::string& name, const double val);

			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalString(const std::string& name, const std::string& val);

			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual int getGlobalInt(const std::string& name) const;

			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual double getGlobalDouble(const std::string& name) const;

			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual std::string getGlobalString(const std::string& name) const;

			static int set_next_mapfile(lua_State *L);
			static int set_next_videofile(lua_State *L);

			lua_State* getState() { return L; }
		private:

			lua_State *L;
			void _initLua();

	};
}
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
