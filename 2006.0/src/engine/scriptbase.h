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
#ifndef FIFE_SCRIPTBASE_H
#define FIFE_SCRIPTBASE_H

#include <string>
#include "exception.h"

namespace FIFE
{
	/* a derived Exception class for script related errors. */
	class ScriptException : public Exception 
	{
		public: 
			inline ScriptException(const std::string& msg) \
				: Exception ( "Script-Exception: " + msg) {};
	};

	// I need a few forward declarations
	class MapView;

	/* needed to allow the abstract class to hold a pointer
	 * to the implementing instance. */
	class ScriptEngine;

	/* the base class of all scriptengines.
	 *
	 * Note:
	 * * had to remove the singleton and reimplement functionality :-(
	 *   this is the only way I got it to compile + work; why does
	 *   g++ complain about undefined references to member functions
	 *   declared 'virtual'? the derived class implements them alright,
	 *   but I get linking errors without the 'empty' implementations
	 *   in this class?
	 *   
	 * * scriptbase.cpp implements the static 'start'; new engine
	 *   implementations should be added there.
	 */ 

	/* * a few things are sub-optimal: like I *need* a pointer to
	 *   a (better: the!) MapView instance to toggle some switches; where
	 *   to get it? Currently gstest.cpp registers the mapview with the
	 *   engine; for now there is only ONE mapview 'instance' available
	 *   to scripts. Are multiple maps (at the same time; multiple mapviews
	 *   in c++ are possible, though you have to 'register' the active one;
	 *   FIXME: is there always at least one mapview?) required?
	 */
	class ScriptEngine { 
		public:
			virtual ~ScriptEngine();

			// derived classes should implement all of these (including start(...))

			// The base class handles the singleton stuff or creates a derived 
			// class instance.
			// Derived classes should implement it to load a 'basic library' file
			// on startup; a zero length filename strings should be ok, too!
			static ScriptEngine* start(const std::string scriptFile);

			virtual void stop() {};
			static ScriptEngine* instance();

			// c++ -> script declarations (of this! class)
			virtual void runFile(const std::string filename) {};
			virtual void runString(const char* string) {};

			virtual void setGlobalInt(const std::string name, const int val) {};
			virtual void setGlobalDouble(const std::string name, const double val) {};
			virtual void setGlobalString(const std::string name, const std::string val) {};
			virtual int getGlobalInt(const std::string name) { return 0;}
			virtual double getGlobalDouble(const std::string name) { return 0.0f;}
			virtual const char* getGlobalString(const std::string name) { return NULL;}

			// c++ only ?
			virtual void registerMapViewInstance(MapView* currentMap) {};
			virtual void setActiveMapElevation(size_t) {};

			std::string implementationName;
		protected:
			static ScriptEngine* m_instance;

	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
