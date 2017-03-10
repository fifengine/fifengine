/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_LOGGER_H
#define FIFE_LOGGER_H

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
#include "util/base/fife_stdint.h"

#include "modules.h"

#ifdef LOG_ENABLED

/** Logs given message with log level "debug" using given logger instance
 */
#define FL_DBG(logger, msg) do { if (FIFE::LogManager::instance()->isVisible(logger.getModule())) logger.log(FIFE::LogManager::LEVEL_DEBUG, msg); } while(0)

/** Logs given message with log level "log" using given logger instance
 */
#define FL_LOG(logger, msg) do { if (FIFE::LogManager::instance()->isVisible(logger.getModule())) logger.log(FIFE::LogManager::LEVEL_LOG, msg); } while(0)

/** Logs given message with log level "warning" using given logger instance
 */
#define FL_WARN(logger, msg) do { if (FIFE::LogManager::instance()->isVisible(logger.getModule())) logger.log(FIFE::LogManager::LEVEL_WARN, msg); } while(0)

/** Logs given message with log level "error" using given logger instance
 */
#define FL_ERR(logger, msg) do { if (FIFE::LogManager::instance()->isVisible(logger.getModule())) logger.log(FIFE::LogManager::LEVEL_ERROR, msg); } while(0)

/** Logs given message with log level "pacic" using given logger instance.
 * Causes also program to abort
 */
#define FL_PANIC(logger, msg) do { if (FIFE::LogManager::instance()->isVisible(logger.getModule())) logger.log(FIFE::LogManager::LEVEL_PANIC, msg); } while(0)

#else
// empty definitions in case logs are turned off for speed
#define FL_DBG(logger, msg)
#define FL_LOG(logger, msg)
#define FL_WARN(logger, msg)
#define FL_ERR(logger, msg)
#define FL_PANIC(logger, msg)
#endif

namespace FIFE {

	/** Helper class to create log strings out from separate parts
	 * Usage: LMsg("some text") << variable << ", " << other variable
	 */
	class LMsg {
	public:
		LMsg(const std::string& msg=""): str(msg) {}
		~LMsg() {}

		template <typename T> LMsg& operator<<(const T& t) {
			std::ostringstream stream;
			stream << t;
			str += stream.str();
			return *this;
		}

		std::string str;
	};

	/** Logmanager takes care of log filtering and output direction
	 */
	class LogManager {
	public:
		/** Loglevel is used to set a treshold for output messages + related filter
		 * E.g. in case log message has LEVEL_WARN, but the filter treshold is LEVEL_ERROR,
		 * log message is not outputted
		 */
		enum LogLevel {
			LEVEL_DEBUG = 0,
			LEVEL_LOG   = 1,
			LEVEL_WARN  = 2,
   			LEVEL_ERROR = 3,
   			LEVEL_PANIC = 4
		};

		/** Returns instance to log manager. Log manager is a singleton class
		 */
		static LogManager* instance();

		/** Destructor
		 */
		~LogManager();

		/** Logs given message
		 * @param level level of this log (e.g. warning)
		 * @param module module where this log message is coming from. Modules are defined in modules.h-file
		 * @param msg message to log
		 * @note do not use this method directly, instead use FL_WARN (or any other FL_XXX) macro
		 */
		void log(LogLevel level, logmodule_t module, const std::string& msg);

		/** Sets currently used level filter.
		 * For usage, @see LogManager::LogLevel
		 */
		void setLevelFilter(LogLevel level);

		/** Gets currently used level filter.
		 * @see LogManager::LogLevel
		 */
		LogLevel getLevelFilter();

		/** Adds visible module into logmanager
		 * Module corresponds some module in the engine. Modules may contain other modules.
		 * Modules and their structure is defined in file modules.h.
		 * In case module is not visible, LogManager filters corresponding log messages
		 * from output. In case some lower-level module is set visible, it also sets
		 * all upper level modules visible
		 * @param module module to set visible
		 */
		void addVisibleModule(logmodule_t module);

		/** Removes visible module, @see addVisibleModule
		 */
		void removeVisibleModule(logmodule_t module);

		/** Removes all visible modules, @see addVisibleModule
		 */
		void clearVisibleModules();

		/** Tells if given module is visible
		 */
		bool isVisible(logmodule_t module);

		/** Sets LogManager to log to prompt
		 */
		void setLogToPrompt(bool logtoprompt);

		/** Returns if LogManager is set to log to prompt
		 */
		bool isLogToPrompt();

		/** Sets LogManager to log to a file
		 */
		void setLogToFile(bool logtofile);

		/** Returns if LogManager is set to log to a file
		 */
		bool isLogToFile();

		/** Gets display name for given module id
		 * E.g. LM_AUDIO -> "Audio"
		 */
		std::string getModuleName(logmodule_t module);

	private:
		void validateModule(logmodule_t m);

		// hidden constructor for singleton
		LogManager();
		// validates if definitions in module.h are valid
		void validateModuleDescription(logmodule_t module);

		// singleton instance
		static LogManager* m_instance;
		// current filter level
		LogLevel m_level;
		// visibility array for modules
		bool m_modules[LM_MODULE_MAX];
		// used during module description validation to check cycles in hierarchy
		std::vector<logmodule_t> module_check_stack;

		bool m_logtofile;
		bool m_logtoprompt;

		std::ofstream* m_logfile;
	};

	/** Create a Logger instance to communicate with LogManager
	 * Logger stores information about the current module thus reducing
	 * the typing needed for individual traces
	 * Common way of doing things is to instantiate a static Logger on
	 * top of .cpp file and then use that in .cpp-file's methods
	 */
	class Logger {
	public:
		/** Creates new logger and associates it with given module
		 */
		Logger(logmodule_t module);

		/** Destructor
		 */
		~Logger();

		/** logs given message with given log level
		 */
		void log(LogManager::LogLevel level, const std::string& msg);

		/** logs given message with given log level.
		 * Message is wrapped into LMsg instance for easy formatting
		 */
		void log(LogManager::LogLevel level, const LMsg& msg);

		/** gets module where this logger is associated to
		 */
		inline logmodule_t getModule() const { return m_module; }

	private:
		logmodule_t m_module;
	};

	/** Helper for printing a pointer
	 *
	 * This is a helper structure that allows printing any kind of pointer
	 * on (hopefully) any platform in hex, kind of like the %p format
	 * string of printf.
	 *
	 * The mechanism is used by calling something like:
	 *  somestream << pprint(ptr);
	 **/
	struct pprint {
		void* p;
		pprint( void* _p ) : p(_p) {}
	};
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
			<< reinterpret_cast<uint64_t>( p.p );

		return s;
	}
}


#endif
