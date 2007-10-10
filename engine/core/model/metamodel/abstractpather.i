%module model
%{
#include "model/metamodel/abstractpather.h"
%}

namespace FIFE {
	class AbstractPather {
	public:
		virtual ~AbstractPather();
	private:
		AbstractPather();
	};
}
