// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODELCOORDS_H
#define FIFE_MODELCOORDS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/structures/point.h"

namespace FIFE
{
    using AudioSpaceCoordinate = DoublePoint3D;
    using ExactModelCoordinate = DoublePoint3D;
    using ModelCoordinate      = Point3D;
} // namespace FIFE

#endif
