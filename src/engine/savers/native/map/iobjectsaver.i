// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "savers/native/map/iobjectsaver.h"
#include "savers/native/map/ianimationsaver.h"
%}

namespace FIFE {
	%feature("director") IObjectSaver;
}

%include "savers/native/map/iobjectsaver.h"
