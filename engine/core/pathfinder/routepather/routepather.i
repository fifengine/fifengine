%module pather
%{
#include "pathfinder/routepather/routepather.h"
%}

%include "model/metamodel/abstractpather.i"

namespace FIFE {
	%feature("notabstract") RoutePather;
	class RoutePather : public AbstractPather {
	public:
		RoutePather();
		virtual ~RoutePather();
		std::string getName() const;
	};
}
