// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "loaders/native/map/ianimationloader.h"
#include "loaders/native/map/animationloader.h"
%}

%include "video/video.i"

namespace FIFE {
	%feature("director") IAnimationLoader;
}

%include "loaders/native/map/ianimationloader.h"
%include "loaders/native/map/animationloader.h"
