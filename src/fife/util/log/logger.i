// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "util/log/logger.h"
%}
%include "modules.h"

%template(moduleVector) std::vector<logmodule_t>;

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