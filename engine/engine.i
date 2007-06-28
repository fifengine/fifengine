%module engine
%{
#include "engine.h"
%}

%include "util/exception.i"

namespace FIFE {
	class Engine {
		Engine();
		virtual ~Engine();
	};
}