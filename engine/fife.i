%module fife
%{
#include "engine.h"
%}

%include "util/exception.i"
%include "util/settingsmanager.i"
%include "util/log.i"
%include "util/time/timemanager.i"
%include "util/time/timer.i"

namespace FIFE {
	class Engine {
	public:
		Engine();
		virtual ~Engine();
	};
}
