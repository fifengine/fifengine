%module fife
%{
#include "model/metamodel/abstractvisual.h"
%}

namespace FIFE {
	class AbstractVisual {
	public:
		virtual ~AbstractVisual();
	private:
		AbstractVisual();
	};
}
