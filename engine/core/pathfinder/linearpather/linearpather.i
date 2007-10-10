%module pather
%{
#include "pathfinder/linearpather/linearpather.h"
%}
%include "model/metamodel/abstractpather.i"

namespace FIFE {
	%feature("notabstract") LinearPather;
	class LinearPather: public AbstractPather {
	public:
		LinearPather();
		virtual ~LinearPather();
	};
}
