// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "pathfinder/route.h"
%}

%template(LocationList) std::list<FIFE::Location>;
