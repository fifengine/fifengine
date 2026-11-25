// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_LOADER_H
#define FIFE_GLCORE_LOADER_H

#include <cstdint>

#include "platform.h"

namespace FIFE
{
    enum class GLProfile : uint8_t;

    /** Loads the generated glad entry points for the active profile. */
    bool glcoreLoadProcs(GLProfile profile);
} // namespace FIFE

#endif
