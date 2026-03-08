// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODELCOORDS_H
#define FIFE_MODELCOORDS_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/point.h"

namespace FIFE
{
    typedef DoublePoint3D AudioSpaceCoordinate;
    typedef DoublePoint3D ExactModelCoordinate;
    typedef Point3D ModelCoordinate;
} // namespace FIFE

#endif