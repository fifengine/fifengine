%module fife
%{
#include "model/metamodel/abstractpather.h"
%}

namespace FIFE {
	class Map;
	
	class AbstractPather {
	public:
		virtual ~AbstractPather();
		virtual std::string getName() const = 0;
	private:
		AbstractPather();
	};
}
