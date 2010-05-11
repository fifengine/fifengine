/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

// Standard C++ library includes
#include <algorithm>
#include <iomanip>
#include <fstream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
// #include "guichan_addon/console.h"
#include "modules.h"
#include "logger.h"
#include "util/base/exception.h"

// define the module info relationships structure here, begin
struct ModuleInfo {
	logmodule_t module;
	logmodule_t parent;
	std::string name;
};
MODULE_INFO_RELATIONSHIPS
// end

namespace FIFE {
	LogManager* LogManager::m_instance = NULL;

	Logger::Logger(logmodule_t module):
		m_module(module) {
	}

	Logger::~Logger() {
	}

	void Logger::log(LogManager::LogLevel level, const std::string& msg) {
		LogManager::instance()->log(level, m_module, msg);
	}

	void Logger::log(LogManager::LogLevel level, const LMsg& msg) {
		LogManager::instance()->log(level, m_module, msg.str);
	}

	LogManager* LogManager::instance() {
		if (!m_instance) {
			m_instance = new LogManager();
		}
		return m_instance;
	}

	LogManager::~LogManager() {
		delete m_instance;
	}


	void LogManager::log(LogLevel level, logmodule_t module, const std::string& msg) {
		if (level < m_level) {
			return;
		}
		if (!isVisible(module)) {
			return;
		}
		std::string lvlstr = "";
		switch (level) {
			case LEVEL_DEBUG: lvlstr = "dbg";
			break;

			case LEVEL_LOG: lvlstr = "log";
			break;

			case LEVEL_WARN: lvlstr = "warn";
			break;

			case LEVEL_ERROR: lvlstr = "error";
			break;

			case LEVEL_PANIC: lvlstr = "panic";
			break;

			default: lvlstr = "error";
			break;
		}
		if (m_logtoprompt) {
			std::cout << moduleInfos[module].name << ": " << lvlstr << ": " << msg << std::endl;
		}
		if (m_logtofile) {
			*m_logfile << moduleInfos[module].name << ": " << lvlstr << ": " << msg << std::endl;
		}
		if (level == LEVEL_PANIC) {
			abort();
		}
	}

	void LogManager::setLevelFilter(LogLevel level) {
		m_level = level;
	}

	LogManager::LogLevel LogManager::getLevelFilter() {
		return m_level;
	}

	void LogManager::addVisibleModule(logmodule_t module) {
		validateModule(module);
 		int ind = static_cast<int>(module);
 		m_modules[ind] = true;
 		if (moduleInfos[ind].parent != LM_CORE) {
  			addVisibleModule(moduleInfos[ind].parent);
 		}
	}

	void LogManager::removeVisibleModule(logmodule_t module) {
		validateModule(module);
		m_modules[module] = false;
	}

	void LogManager::clearVisibleModules() {
		for (int i = 0; i < LM_MODULE_MAX; i++) {
			m_modules[i] = false;
		}
	}

	void LogManager::setLogToPrompt(bool log_to_promt) {
		m_logtoprompt = log_to_promt;
	}

	bool LogManager::isLoggingToPrompt() {
		return m_logtoprompt;
	}

	void LogManager::setLogToFile(bool logtofile) {
		if(logtofile){
			m_logfile = new std::ofstream("fife.log");
		}
		else {
			if (m_logfile){
				delete m_logfile;
			}
		}
		m_logtofile = logtofile;
	}

	bool LogManager::isLoggingToFile() {
		return m_logtofile;
	}

	bool LogManager::isVisible(logmodule_t module) {
		if (!m_modules[module]) {
			return false;
		}
		if (moduleInfos[module].parent != LM_CORE) {
 			return isVisible(moduleInfos[module].parent);
		}
		return true;
	}

	LogManager::LogManager():
		m_level(LEVEL_DEBUG),
		module_check_stack(),
		m_logtofile(false),
		m_logtoprompt(false) {
		validateModuleDescription(LM_CORE);
		m_logfile = 0;
		clearVisibleModules();
	}

	void LogManager::validateModule(logmodule_t m) {
		if ((m <= LM_CORE) || (m >= LM_MODULE_MAX)) {
			std::cout << "Invalid module received in LogManager: " << m << ", aborting\n";
			abort();
		}
	}

	void LogManager::validateModuleDescription(logmodule_t module) {
		if (module == LM_CORE) {
			for (int m = static_cast<int>(LM_CORE)+1; m < static_cast<int>(LM_MODULE_MAX); m++) {
				if (moduleInfos[m].module != static_cast<logmodule_t>(m)) {
					std::ostringstream stream;
					stream << m;
					std::string msg = "Log module definition ids do not match in index ";
					msg += stream.str();
					std::cout << msg << std::endl;
					throw InvalidFormat(msg);
				}
				module_check_stack.clear();
				validateModuleDescription(static_cast<logmodule_t>(m));
			}
		} else {
			module_check_stack.push_back(module);
			if (count(module_check_stack.begin(), module_check_stack.end(), module) > 1) {
				throw InvalidFormat("Log module definition hierachy contains cycles");
			}
		}
	}

	std::string LogManager::getModuleName(logmodule_t module) {
		return moduleInfos[module].name;
	}
}
