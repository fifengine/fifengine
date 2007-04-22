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

#ifndef FIFE_SCRIPTBASE_H
#define FIFE_SCRIPTBASE_H

#include <string>
#include "exception.h"
#include "scriptcontainer.h"

namespace FIFE
{
	/** Derived Exception class for script related errors.
	 */
	class ScriptException : public Exception
	{
		public:
			inline ScriptException(const std::string& msg) \
				: Exception ( "Script-Exception: " + msg) {};
	};

	// I need a few forward declarations
	class MapView;

	/* Needed to allow the abstract class to hold a pointer
	 * to the implementing instance. */
	class ScriptEngine;

	/** Base class of all scriptengines.
	 *
	 * One original goal was the ability to change the backend at 
	 * compile-time; I have chosen \em inheritance to solve this.
	 *
	 * \note
	 * \li had to remove the singleton and reimplement functionality :-(
	 *   this is the only way I got it to compile + work;
	 *   [above is rather old; I should try it again...]
	 *   
	 * \li scriptbase.cpp implements the static 'start'; new engine
	 *   implementations should be added there.
	 *
	 * \li All backends should implement a number of generic functions;
	 * furthermore I put all other (required) functions here until better
	 * solutions from them can be found.
	 * 
	 * [older docs]
	 * \li a few things are sub-optimal: like I *need* a pointer to
	 *   a (better: the!) MapView instance to toggle some switches; where
	 *   to get it? Currently gstest.cpp registers the mapview with the
	 *   engine; for now there is only ONE mapview 'instance' available
	 *   to scripts. Are multiple maps (at the same time; multiple mapviews
	 *   in c++ are possible, though you have to 'register' the active one;
	 *   FIXME: is there always at least one mapview?) required?
	 */
	class ScriptEngine {
		public:
			/** Destructor calls \c stop function on exit.
			 *
			 * \note \em virtual for the usual reasons
			 */
			virtual ~ScriptEngine();

			// derived classes should implement all of these (including start(...))

			// The base class handles the singleton stuff or creates a derived
			// class instance.
			// Derived classes should implement it to load a 'basic library' file
			// on startup; a zero length filename strings should be ok, too!
			
			/** Use this to create the singleton instance the \b first time.
			 *
			 * @param filename Path to a initial scriptfile
			 * @return Pointer to ScriptEngine instance
			 *
			 * \note The idea is to use the initial file to define whatever you want;
			 * you can't actually run anything (because the corresponding c++ objects
			 * aren't ready yet). The filename may also be empty (no initial script).
			 *
			 * \note The engine should be in a well-defined state even if \c start is
			 * called multiple times. This means you can \b restart the scriptengine
			 * by calling \c start again.
			 */
			static ScriptEngine* start(const std::string scriptFile);

			/** Clear the script state (remove all script variables & objects).
			 */
			virtual void stop() {};
			
			/** \em singleton-like access to the instance.
			 *
			 * @return Pointer to actual scriptbackend instance.
			 *
			 * \note You can create the instance with this function as well
			 * (it calls \c start with an empty string); the idea was
			 * to use this to access the instance after it has been created
			 * the first time.
			 *
			 * \see start function
			 */ 
			static ScriptEngine* instance();

			// c++ -> script declarations (of this! class)
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
			virtual void runFile(const std::string filename) {};
			
			/** Run a string.
			 * @param string with valid code (for the current backend)
			 *
			 * \note The quick-n-dirty way to run something.
			 */
			virtual void runString(const char* string) {};
			
			/** Call a script-function.
			 * @param Name of the function to call.
			 *
			 * \note Used for simple callbacks (the GUI); no arguments or return
			 * values, but a lot faster than calling \c runString().
			 */
			virtual void callFunction(const char* name) {};

			/** Execute a ScriptContainer
			 *
			 * @param script The ScriptContainer to run
			 */
			virtual void execute(const ScriptContainer& sc);

			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalInt(const std::string name, const int val) {};
			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalDouble(const std::string name, const double val) {};
			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalString(const std::string name, const std::string val) {};
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual int         getGlobalInt(const std::string name) { return 0;}
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual double      getGlobalDouble(const std::string name) { return 0.0f;}
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual const char* getGlobalString(const std::string name) { return NULL;}

			// c++ only ?
			/** Hack to export the MapView to the scriptengine.
			 *
			 * MapView is not a \em singleton but I need to access it from the scripts;
			 * this is a primitive solution.
			 *
			 * @param Pointer to valid MapView instance
			 *
			 * \bug Scripts doing something with the mapview while a) there is none
			 * or b) the pointer is no longer valid WILL die a horrible death ;-)
			 */
			virtual void registerMapViewInstance(MapView* currentMap) {};

			/** Experimental: IGNORE THIS!
			 * 
			 * \param idx Valid index into the currently active map; requires active MapView!
			 * 
			 * test-export of game-objects into the script-vm
			 */
			virtual void setActiveMapElevation(size_t) {};

			std::string implementationName;
		protected:
			static ScriptEngine* m_instance;

	};
}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
