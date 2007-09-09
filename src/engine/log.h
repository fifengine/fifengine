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
#ifndef FIFELOG_H
#define FIFELOG_H

#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>

#define Warn( _type ) Log( (_type) , Log::LEVEL_WARN )
#define Debug( _type ) Log( (_type) , Log::LEVEL_DEBUG )

namespace FIFE {
	class Console;
	/** Log class
	 * 
	 * We are using a two fold approach: 
	 * 1. All output has to use a debugclass and the user of the prog. can decide which types
	 * he wants to see.
	 * Those debugclasses could f.eg. be something like "mlfallout" for maploaderfallout etc.
	 * In that case the programmer would write
	 * \code
	 * Log("mlfallout") << "one line of debug info " << 3434 << " text continued;
	 * \endcode
	 * If no type is specified, "common" is used.
	 * 
	 * 2. Every logging has an log level attached to it, by the macros Warn(),Debug() and Log()
	 * itself.
	 * \code
	 * Warn("gui") << "This is a Warning, and points to possibly serious faults!";
	 * Log("animation") << "Log common, but notable events, like an engine starting up.";
	 * Debug("palanim") << "Give debugging information, like detecting an pal animation.";
	 * \endcode
	 *
	 * If the --debug flag is given, everything after it is assumed to be a debugclass that
	 * should be shown.
	 * If not added by the --debug flag behaviour depends on the log level, which can be set either
	 * in the settings file or in the console by log.setLogLevel( logLevel )
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
			Log(const std::string& type = "common",const type_log_level& logLevel = LEVEL_LOG );
			~Log();

			template <typename T> Log& operator<<(const T& t) {
				if (m_show){
					std::ostringstream stream;
					stream << t;
					m_logstring += stream.str();
				}
				return *this;
			}

			static void addVisibleType(const std::string& type);
			static bool isVisibleType(const std::string& type);
/* 
			static void removeVisibleType(const std::string& type);
			static void setShowAll(bool toggle);
			static void setLogRecording(bool toggle);
*/
			static void parseCmdLine(int argc, char* argv[]);
			static void setConsole(Console* console);
			static void setLogLevel(type_log_level log_level );
		private:
			void logString(const std::string& str);
			static bool isVisibleLogLevel(const type_log_level& log_level );
			static void initialize();

			static bool m_showall;
			static int m_logLevelTreshold;
			static Console* m_console;

			bool m_show;
			std::string m_logstring;

			typedef std::vector<std::string> type_dbgtypes;
//			typedef std::list<struct log_entry> type_logentries;
			static type_dbgtypes m_dbgtypes;
/*			static type_logentries m_logentries;

			struct log_entry {
				unsigned ticks;
				std::string type;
				std::string message;
			};
*/
	};
}

#endif
