%module pather
%{
#include "pathfinder/realtimepather/realtimepather.h"
%}

%include "model/metamodel/abstractpather.i"

namespace FIFE {
	%feature("notabstract") RealTimePather;
	class RealTimePather : public AbstractPather {
	public:
		RealTimePather();
		virtual ~RealTimePather();
		std::string getName() const;
	};
}
