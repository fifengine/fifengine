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
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

%module logger
%{
#include "util/logger.h"
%}
%include "modules.h"

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
	};
	
	class Logger {
	public:
		Logger(logmodule_t module);
		~Logger();
		void log(LogManager::LogLevel level, const std::string& msg);
	};
}