%module model
%{
#include "model/metamodel/modelcoords.h"
%}

%include "model/metamodel/modelcoords.h"

namespace FIFE {
	%template(ModelCoordinate) PointType3D<int>;
	%template(ExactModelCoordinate) PointType3D<double>;
}