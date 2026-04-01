// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_UTIL_FIFE_STDINT_H
#define FIFE_UTIL_FIFE_STDINT_H

// Standard C++ library includes

// Platform specific includes
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
    #include <stdint.h>
#endif

// 3rd party library includes
#include <SDL_endian.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

// SDL masks for SDL_CreateRGBSurface
namespace FIFE
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    const uint32_t RMASK = 0xff000000;
    const uint32_t GMASK = 0x00ff0000;
    const uint32_t BMASK = 0x0000ff00;
    const uint32_t AMASK = 0x000000ff;
#else
    const uint32_t RMASK = 0x000000ff;
    const uint32_t GMASK = 0x0000ff00;
    const uint32_t BMASK = 0x00ff0000;
    const uint32_t AMASK = 0xff000000;
#endif
    const uint32_t NULLMASK = 0x00000000;
} // namespace FIFE

#endif // FIFEINT_H
