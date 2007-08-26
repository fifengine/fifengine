%module pather
%{
#include "pathfinder/linearpather/linearpather.h"
%}

%include "model/metamodel/abstractpather.h"

namespace FIFE { namespace model {
	class LinearPather: public AbstractPather {
	public:
		LinearPather();
		virtual ~LinearPather();
	};
}}
