%module fife
%{
#include "engine.h"
%}

%include "util/exception.i"
%include "util/settingsmanager.i"
%include "util/log.i"

namespace FIFE {
	class Engine {
	public:
		Engine();
		virtual ~Engine();
	};
}