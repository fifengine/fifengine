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

#ifndef FIFE_SCRIPTENGINE_H
#define FIFE_SCRIPTENGINE_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "exception.h"

// FIXME: Weird coupling with map/ stuff.
namespace FIFE {

	class ScriptException : public Exception {
		public:
			ScriptException(const std::string& msg) : Exception ( "Script-Exception: " + msg) {
			};
	};

	namespace map { class View; }
	class ScriptContainer;
	class ScriptEngine {
		public:
			ScriptEngine(const std::string& name);
			virtual ~ScriptEngine();

			virtual void init() = 0;
			virtual void deinit() = 0;

			const std::string& name() const;

			// derived classes should implement all of these (including start(...))

			// The base class handles the singleton stuff or creates a derived
			// class instance.
			// Derived classes should implement it to load a 'basic library' file
			// on startup; a zero length filename strings should be ok, too!

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
			virtual void runFile(const std::string& filename) = 0;
			
			/** Run a string.
			 * @param string with valid code (for the current backend)
			 *
			 * \note The quick-n-dirty way to run something.
			 */
			virtual void runString(const std::string& string) = 0;
			
			/** Call a script-function.
			 * @param Name of the function to call.
			 *
			 * \note Used for simple callbacks (the GUI); no arguments or return
			 * values, but a lot faster than calling \c runString().
			 */
			virtual void callFunction(const std::string& name) = 0;

			/** Execute a ScriptContainer
			 *
			 * @param script The ScriptContainer to run
			 */
			virtual void execute(const ScriptContainer& sc);

			/** Completely boring mutator for global script-variables.
			 */
			
			virtual void setGlobalInt(const std::string& name, const int val) = 0;
			/** Completely boring mutator for global script-variables.
			 */
			
			virtual void setGlobalDouble(const std::string& name, const double val) = 0;
			
			/** Completely boring mutator for global script-variables.
			 */
			virtual void setGlobalString(const std::string& name, const std::string& val) = 0;
			
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual int getGlobalInt(const std::string& name) const = 0;
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual double getGlobalDouble(const std::string& name) const = 0;
			
			/** Revolutionary concept: accessor function for script variables.
			 */
			virtual std::string getGlobalString(const std::string& name) const = 0;

		private:
			std::string m_name;

	};

}

#endif
