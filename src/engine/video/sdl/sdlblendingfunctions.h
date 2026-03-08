// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H
#define FIFE_VIDEO_RENDERBACKENDS_SDL_SDLBLENDINGFUNCTIONS_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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
    void SDL_BlendRow_RGBA8_to_RGBA8(const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA8_to_RGB8(const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA8_to_RGB565(const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n);

    /** Blends one row of n pixels from src with n pixels of dst.
     *
     * @param src Source.
     * @param dst Destiny.
     * @param alpha Level of alphablending.
     * @param n Number of pixels.
     */
    void SDL_BlendRow_RGBA4_to_RGB565(const uint8_t* src, uint8_t* dst, uint32_t alpha, int32_t n);

} // namespace FIFE

#endif