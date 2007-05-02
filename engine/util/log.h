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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef FIFE_LOG_H
#define FIFE_LOG_H

// Standard C++ library includes
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
// 	class Console;

	/** Helper for printing a pointer
	 *
	 * This is a helper structure that allows printing any kind of pointer
	 * on (hopefully) any platform in hex, kind of like the %p format
	 * string of printf.
	 *
	 * The mechanism is used by calling something like:
	 *
	 *  somestream << pprint(ptr);
	 * */
	struct pprint {
		void* p;

		pprint( void* _p ) : p(_p) {}
	};

	/** Log class.
	 *
	 * We are using a two fold approach:
	 * 1. All output has to use a debugclass and the user of the prog. can decide which types
	 * he wants to see.
	 * Those debugclasses could f.eg. be something like "mlfallout" for maploaderfallout etc.
	 * In that case the programmer would write
	 * @code
	 * Log("mlfallout") << "one line of debug info " << 3434 << " text continued;
	 * @endcode
	 * If no type is specified, "common" is used.
	 *
	 * 2. Every logging has a log level attached to it, by the macros Warn(), Debug() and Log()
	 * itself.
	 * @code
	 * Warn("gui") << "This is a Warning, and points to possibly serious faults!";
	 * Log("animation") << "Log common, but notable events, like an engine starting up.";
	 * Debug("palanim") << "Give debugging information, like detecting a pal animation.";
	 * @endcode
	 *
	 * If the --debug flag is given, everything after it is assumed to be a debugclass that
	 * should be shown.
	 * If not added by the --debug flag behaviour depends on the log level, which can be set either
	 * in the settings file or in the console by log.setLogLevel(logLevel) (defunct)
	 * @warning This class accesses the Settingsmanager in it's initializing call to
	 * parseCmdLine(). It has to be called after Engine::instance() is called the first time.
	 */
	class Log {
		public:
			enum type_log_level {
				LEVEL_MAX   = 3,
				LEVEL_DEBUG = 2,
				LEVEL_LOG   = 1,
				LEVEL_WARN  = 0
			};
		public:
			/** Constructor.
			 *
			 * Decides whether the object being logged should be logged at all based
			 * on its log level and type. It also places the value of type at the 
			 * beginning of the log string if it is to be logged.
			 *
			 * @param type String to be placed at the beginning of the log string.
			 * @param loglevel The log level of this entry.
			 */
			Log(const std::string& type = "common", const type_log_level& logLevel = LEVEL_LOG );

			/** Destructor.
			 *
			 * Outputs the final log string to the standard output. 
			 */
			~Log();

			/** Allow the log item to operate like std::cout.
			 *
			 * @param t The value to be appended to the string.
			 * @return A reference to self.
			 */
			template <typename T> Log& operator<<(const T& t) {
				if (m_show){
					std::ostringstream stream;
					stream << t;
					m_logstring += stream.str();
				}
				return *this;
			}

			/** Register a value of type as visible.
			 *
			 * @param type The type string to be registered.
			 */ 
			static void addVisibleType(const std::string& type);

			/** Check if a value of type is visible.
			 *
			 * @param type The type string to be checked.
			 * @return True if the string is visible.
			 */
			static bool isVisibleType(const std::string& type);
/*
			static void removeVisibleType(const std::string& type);
			static void setShowAll(bool toggle);
			static void setLogRecording(bool toggle);
*/
			/** Initialise the log settings.
			 *
			 * Checks for --debug in the command line and adds
			 * following parameters as visible types. Also calls
			 * initialize().
			 *
			 * @param args List of args.
			 */
			static void parseCmdLine(const std::map<std::string,std::vector<std::string> >& args);
// 			static void setConsole(Console* console);

			/** Set log level threshold.
			 *
			 * @param log_level The level below which is visible.
			 */
			static void setLogLevel(type_log_level log_level);
		
		private:
			/** Get the default log level from the setting file.
			 */
			static void initialize();

			/** Prints the log string. Called by destructor.
			 *
			 * @param str The string to be printed to the standard output.
			 */
			void logString(const std::string& str);

			/** Check if log level is below threshhold.
			 *
			 * @param log_level Log level to be checked.
			 * @return True is level is visible.
			 */
			static bool isVisibleLogLevel(const type_log_level& log_level );

			// True if all log messages are to be outputted.
			static bool m_showall;
			// Log level below which is visible.
			static int m_logLevelTreshold;
// 			static Console* m_console;

			// True if the current log is to be outputted.
			bool m_show;
			// The log string.
			std::string m_logstring;

			typedef std::set<std::string> type_dbgtypes;
//			typedef std::list<struct log_entry> type_logentries;

			// Vector containing the visible type values.
			static type_dbgtypes m_dbgtypes;

/*			static type_logentries m_logentries;

			struct log_entry {
				unsigned ticks;
				std::string type;
				std::string message;
			};
*/
	};

	/** Shortcut to warning-level log output.
	 * */
	inline Log Warn( const std::string& type ) {
		return Log( type, Log::LEVEL_WARN );
	}

	/** Shortcut to debug-level log output.
	 * */
	inline Log Debug( const std::string& type ) {
		return Log( type, Log::LEVEL_DEBUG );
	}
}

namespace std {
	/** Print a pprint object to an ostream.
	 *
	 * This is pure Stroustrup, overloading the ostream operator<< to print
	 * a formatted pointer from a pprint object to an ostream.
	 *
	 * \param s output stream
	 * \param p pointer to print
	 * \return reference to the modified stream
	 * */
	template <class Ch, class Tr>
	basic_ostream<Ch,Tr>& operator<<( basic_ostream<Ch,Tr>& s, const FIFE::pprint& p ) {
		s << "0x"
			<< hex << setw( 2*sizeof(void*) ) << setfill('0')
			<< reinterpret_cast<unsigned long>( p.p );

		return s;
	}
}

#endif
