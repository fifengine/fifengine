// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "savers/native/map/imapsaver.h"
%}

namespace FIFE {
	%feature("director") IMapSaver;
}

%include "savers/native/map/imapsaver.h"
