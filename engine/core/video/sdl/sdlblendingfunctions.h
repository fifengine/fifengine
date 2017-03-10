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

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

namespace FIFE {

	/** Blends one row of n pixels from src with n pixels of dst.
 	 *
 	 * @param src Source.
 	 * @param dst Destiny.
 	 * @param alpha Level of alphablending.
 	 * @param n Number of pixels.
 	 */
	void SDL_BlendRow_RGBA8_to_RGBA8( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n );

	/** Blends one row of n pixels from src with n pixels of dst.
 	 *
 	 * @param src Source.
 	 * @param dst Destiny.
 	 * @param alpha Level of alphablending.
 	 * @param n Number of pixels.
	 */
	void SDL_BlendRow_RGBA8_to_RGB8( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n );

	/** Blends one row of n pixels from src with n pixels of dst.
 	 *
 	 * @param src Source.
 	 * @param dst Destiny.
 	 * @param alpha Level of alphablending.
 	 * @param n Number of pixels.
 	 */
	void SDL_BlendRow_RGBA8_to_RGB565( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n );

	/** Blends one row of n pixels from src with n pixels of dst.
 	 *
 	 * @param src Source.
 	 * @param dst Destiny.
 	 * @param alpha Level of alphablending.
 	 * @param n Number of pixels.
 	 */
	void SDL_BlendRow_RGBA4_to_RGB565( const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n );

}

#endif
