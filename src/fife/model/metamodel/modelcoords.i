// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/modelcoords.h"
%}

%include "model/metamodel/modelcoords.h"
%include "util/structures/utilstructures.i"

namespace FIFE {
	%template(AudioSpaceCoordinate) PointType3D<double>;
	%template(ModelCoordinate) PointType3D<int32_t>;
	%template(ExactModelCoordinate) PointType3D<double>;
}