// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "loaders/native/map/imaploader.h"
%}

namespace FIFE {
	%feature("director") IMapLoader;
}

%include "loaders/native/map/imaploader.h"
