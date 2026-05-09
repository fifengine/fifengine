// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_UTIL_TIME_SDLTIMECOMPAT_H
#define FIFE_UTIL_TIME_SDLTIMECOMPAT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes

namespace FIFE
{
    namespace SDLTimeCompat
    {
        // SDL3 uses Uint64 ticks. These helpers keep explicit legacy wrap semantics
        // where FIFE APIs still expose 32-bit time values.
        [[nodiscard]] inline Uint32 toUint32Ticks(Uint64 ticks)
        {
            return static_cast<Uint32>(ticks);
        }

        [[nodiscard]] inline int32_t toInt32Ticks(Uint64 ticks)
        {
            return static_cast<int32_t>(toUint32Ticks(ticks));
        }

        [[nodiscard]] inline Uint64 fromLegacy32Ticks(uint32_t ticks)
        {
            return static_cast<Uint64>(ticks);
        }

        [[nodiscard]] inline Uint64 fromLegacy32Ticks(int32_t ticks)
        {
            return static_cast<Uint64>(static_cast<Uint32>(ticks));
        }

    } // namespace SDLTimeCompat
} // namespace FIFE

#endif
