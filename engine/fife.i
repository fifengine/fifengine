%module fife
%{
#include "engine.h"
%}

%include "util/exception.i"

namespace FIFE {
	class Engine {
	public:
		Engine();
		virtual ~Engine();
	};
}