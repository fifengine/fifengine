// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_UTIL_FIFE_STDINT_H
#define FIFE_UTIL_FIFE_STDINT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#if defined(WIN32) && defined(_MSC_VER)
    #ifndef _SDL_H
typedef signed __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef signed __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
    #endif
#else
    #include <cstdint>
#endif

// 3rd party library includes
#include <SDL3/SDL_endian.h>

// FIFE includes

// SDL masks for SDL_CreateSurface
namespace FIFE
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    uint32_t const RMASK = 0xff000000;
    uint32_t const GMASK = 0x00ff0000;
    uint32_t const BMASK = 0x0000ff00;
    uint32_t const AMASK = 0x000000ff;
#else
    uint32_t const RMASK = 0x000000ff;
    uint32_t const GMASK = 0x0000ff00;
    uint32_t const BMASK = 0x00ff0000;
    uint32_t const AMASK = 0xff000000;
#endif
    uint32_t const NULLMASK = 0x00000000;
} // namespace FIFE

#endif // FIFEINT_H
