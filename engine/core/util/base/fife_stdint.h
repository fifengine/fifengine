/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_UTIL_FIFE_STDINT_H
#define FIFE_UTIL_FIFE_STDINT_H

// Standard C++ library includes

// Platform specific includes
#if defined( WIN32 ) && defined( _MSC_VER )
#ifndef _SDL_H
typedef signed __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef signed __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef signed __int32		int32_t;
typedef unsigned __int32	uint32_t;
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#endif
#else
#include <stdint.h>
#endif

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

// SDL masks for SDL_CreateRGBSurface
namespace FIFE {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const uint32_t RMASK    = 0xff000000;
	const uint32_t GMASK    = 0x00ff0000;
	const uint32_t BMASK    = 0x0000ff00;
	const uint32_t AMASK    = 0x000000ff;
#else
	const uint32_t RMASK    = 0x000000ff;
	const uint32_t GMASK    = 0x0000ff00;
	const uint32_t BMASK    = 0x00ff0000;
	const uint32_t AMASK    = 0xff000000;
#endif
	const uint32_t NULLMASK = 0x00000000;
} //FIFE

#endif // FIFEINT_H
