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
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/
#include <string>
#include "scriptbase.h"
#ifdef SCRIPTENGINE_LUA
#include "luascript/luascript.h"
#endif

namespace FIFE {
	ScriptEngine::~ScriptEngine() {
	}
	ScriptEngine* ScriptEngine::start(const std::string filename) {
		if (m_instance == NULL) {
#ifdef SCRIPTENGINE_LUA
			LuaScript* new_instance = new LuaScript();
			new_instance->start(filename);
			m_instance = new_instance;
#else
			class EmptyScript : public ScriptEngine {
				public:
					~EmptyScript() {}
					void stop() {}

					void runFile(const std::string filename) {}
					void runString(const char* string) {}
					void setGlobalInt(const std::string name, const int val) {}
					void setGlobalDouble(const std::string name, const double val) {}
					void setGlobalString(const std::string name, const std::string val) {}
					int getGlobalInt(const std::string name) { return 0;}
					double getGlobalDouble(const std::string name) { return 0.0f;}
					const char* getGlobalString(const std::string name) { return NULL;}
			};
			m_instance = new EmptyScript();
#endif
			//m_instance->start(filename);
		}
		else {
#ifdef SCRIPTENGINE_LUA
			LuaScript* l_instance = static_cast<LuaScript*>(m_instance);
			l_instance->start(filename);
#endif
		}
		return m_instance;
	}
	ScriptEngine* ScriptEngine::instance() {
		if (m_instance == NULL)
			ScriptEngine::start("");
		return m_instance;
	}

	ScriptEngine* ScriptEngine::m_instance = NULL;
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
