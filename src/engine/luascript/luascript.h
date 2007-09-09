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
			~LuaScript();
			void start(const std::string scriptFile);
			void stop();
			void runFile(const std::string scriptFile);
			void runString(const char *string);

			void setGlobalInt(const std::string name, const int val);
			void setGlobalDouble(const std::string name, const double val);
			void setGlobalString(const std::string name, const std::string val);
			int getGlobalInt(const std::string name);
			double getGlobalDouble(const std::string name);
			const char* getGlobalString(const std::string name);

			void registerMapViewInstance(MapView* currentMap);
			void setActiveMapElevation(size_t);
		private:
			//SINGLEFRIEND(LuaScript);
			lua_State *L;
			void _initLua();

	};
}  
#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
