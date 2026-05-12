// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "loaders/native/map/iobjectloader.h"
#include "loaders/native/map/ianimationloader.h"
#include "loaders/native/map/objectloader.h"
%}

namespace FIFE {
	%feature("director") IObjectLoader;
}

%include "loaders/native/map/iobjectloader.h"
%include "loaders/native/map/objectloader.h"