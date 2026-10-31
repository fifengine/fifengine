// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "model/metamodel/object.h"
%}

%include "util/base/utilbase.i"
%include "model/metamodel/ivisual.i"

// Ignore move assignment operators for nested property classes (SWIG doesn't support them well)
%ignore Object::BasicObjectProperty::operator=;
%ignore Object::MovableObjectProperty::operator=;
%ignore Object::MultiObjectProperty::operator=;

%include "model/metamodel/object.h"