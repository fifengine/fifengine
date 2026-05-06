// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"

namespace FIFE
{

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA8_to_RGBA8(uint8_t const * src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA8_to_RGB8(uint8_t const * src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA8_to_RGB565(uint8_t const * src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA4_to_RGB565(uint8_t const * src, uint8_t* dst, uint32_t alpha, int32_t n);

} // namespace FIFE

#endif
