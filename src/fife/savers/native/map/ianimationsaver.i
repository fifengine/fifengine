// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "savers/native/map/ianimationsaver.h"
%}

namespace FIFE {
	%feature("director") IAnimationSaver;
	%feature("director") AnimationSaver;
}

%include "savers/native/map/ianimationsaver.h"
