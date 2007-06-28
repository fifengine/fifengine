%module log
%{
#include "util/log.h"
%}

%include "std_string.i"

namespace FIFE {
	class Log {
		public:
			enum type_log_level {
				LEVEL_MAX   = 3,
				LEVEL_DEBUG = 2,
				LEVEL_LOG   = 1,
				LEVEL_WARN  = 0
			};
			Log(const std::string& type = "common", const type_log_level& logLevel = LEVEL_LOG );
			~Log();
			static void setLogLevel(type_log_level log_level);
			static void initialize(type_log_level loglevel, bool logtofile, bool logtoprompt);
	};
}
