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
#include "modules.h"


#define LOG_ENABLED

#ifdef LOG_ENABLED
#define FL_DBG(logger, msg) logger.log(LogManager::LEVEL_DEBUG, msg)
#define FL_LOG(logger, msg) logger.log(LogManager::LEVEL_LOG, msg)
#define FL_WARN(logger, msg) logger.log(LogManager::LEVEL_WARN, msg)
#define FL_ERR(logger, msg) logger.log(LogManager::LEVEL_ERROR, msg)
#else
#define FL_DBG(logger, msg)
#define FL_LOG(logger, msg)
#define FL_WARN(logger, msg)
#define FL_ERR(logger, msg)
#endif

namespace FIFE {
	
	class LMsg {
	public:
		LMsg(const std::string& msg=""): m_logstring(msg) {}
		~LMsg() {}
		
		template <typename T> LMsg& operator<<(const T& t) {
			std::ostringstream stream;
			stream << t;
			m_logstring += stream.str();
			return *this;
		}
		
		std::string m_logstring;
	};
	
	class LogManager {
	public:
		enum LogLevel {	
			LEVEL_DEBUG = 0,
			LEVEL_LOG   = 1,
			LEVEL_WARN  = 2,
   			LEVEL_ERROR = 3
		};
		
		static LogManager* instance();
		~LogManager();
	
		void log(LogLevel level, logmodule_t module, const std::string& msg);
		
		void setLevelFilter(LogLevel level);
		LogLevel getLevelFilter();
		
		void addVisibleModule(logmodule_t module);
		void removeVisibleModule(logmodule_t module);
		
		void setLogToPromt(bool log_to_promt);
		bool isLoggingToPromt();
		
		void setLogToFile(bool logtofile);
		bool isLoggingToFile();
		
		bool isEnabled(logmodule_t module);
		
	private:
		LogManager();
		void validateModuleDescription(logmodule_t module);
		
		static LogManager* m_instance;
		LogLevel m_level;
		bool m_modules[LM_MODULE_MAX];
		std::vector<logmodule_t> module_check_stack;
		
		bool m_logtofile;
		bool m_logtoprompt;
		
		std::ofstream* m_flowtracefile;
		std::ofstream* m_logfile;
	};
	
	class Logger {
	public:
		Logger(logmodule_t module);
		~Logger();
		void log(LogManager::LogLevel level, const std::string& msg);
		void log(LogManager::LogLevel level, const LMsg& msg);
	private:
		logmodule_t m_module;
	};
}

#endif
