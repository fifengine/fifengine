// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "loaders/native/map/iatlasloader.h"
#include "loaders/native/map/atlasloader.h"
%}

namespace FIFE {
	%feature("director") IAtlasLoader;
}

%include "loaders/native/map/iatlasloader.h"
%include "loaders/native/map/atlasloader.h"
