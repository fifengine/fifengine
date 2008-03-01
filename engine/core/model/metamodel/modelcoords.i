%module fife
%{
#include "model/metamodel/modelcoords.h"
%}

%include "model/metamodel/modelcoords.h"
%include "util/structures/utilstructures.i"

namespace FIFE {
	%template(ModelCoordinate) PointType3D<int>;
	%template(ExactModelCoordinate) PointType3D<double>;
}