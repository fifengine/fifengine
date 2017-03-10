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

%module fife
%{
#include "util/log/logger.h"
%}
%include "modules.h"

namespace std {
	%template(moduleVector) std::vector<logmodule_t>;
};

namespace FIFE {
	
	
	class LogManager {
	public:
		enum LogLevel {	
			LEVEL_DEBUG = 0,
			LEVEL_LOG   = 1,
			LEVEL_WARN  = 2,
   			LEVEL_ERROR = 3
		};
		~LogManager();
		
		void log(LogLevel level, logmodule_t module, const std::string& msg);
		void setLevelFilter(LogLevel level);
		LogLevel getLevelFilter();
		
		void addVisibleModule(logmodule_t module);
		void removeVisibleModule(logmodule_t module);
		void clearVisibleModules();
		bool isVisible(logmodule_t module);
		
		void setLogToPrompt(bool logtoprompt);
		bool isLogToPrompt();
		
		void setLogToFile(bool logtofile);
		bool isLogToFile();
		std::string getModuleName(logmodule_t module);
		
	private:
		LogManager();
	};
	
	class Logger {
	public:
		Logger(logmodule_t module);
		~Logger();
		void log(LogManager::LogLevel level, const std::string& msg);
	};
}
