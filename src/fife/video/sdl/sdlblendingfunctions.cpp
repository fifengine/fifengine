// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "sdlblendingfunctions.h"

// Standard C++ library includes
#include <cstddef>
#include <span>

// 3rd party library includes

// FIFE includes
namespace FIFE
{

    struct ColorRGB8
    {
            uint8_t r, g, b;
    };

    struct ColorRGBA8
    {
            uint8_t r, g, b, a;
    };

    void SDL_BlendRow_RGBA8_to_RGBA8([[maybe_unused]] uint8_t const * src, uint8_t* dst, uint32_t alpha, size_t n)
    {
        auto srcSpan = std::span<ColorRGBA8 const>{static_cast<ColorRGBA8 const *>(static_cast<void const *>(src)), n};
        auto dstSpan = std::span<ColorRGBA8>{static_cast<ColorRGBA8*>(static_cast<void*>(dst)), n};

        for (size_t i = 0; i < n; ++i) {
            uint32_t const aMulA = alpha * srcSpan[i].a;

            if (aMulA != 0U) {
                uint32_t const OneMin_aMulA = 65535 - aMulA;
                dstSpan[i].r = static_cast<uint8_t>(((aMulA * srcSpan[i].r) + (OneMin_aMulA * dstSpan[i].r)) >> 16);
                dstSpan[i].g = static_cast<uint8_t>(((aMulA * srcSpan[i].g) + (OneMin_aMulA * dstSpan[i].g)) >> 16);
                dstSpan[i].b = static_cast<uint8_t>(((aMulA * srcSpan[i].b) + (OneMin_aMulA * dstSpan[i].b)) >> 16);
            }
        }
    }

    void SDL_BlendRow_RGBA8_to_RGB8([[maybe_unused]] uint8_t const * src, uint8_t* dst, uint32_t alpha, size_t n)
    {
        auto srcSpan = std::span<ColorRGBA8 const>{static_cast<ColorRGBA8 const *>(static_cast<void const *>(src)), n};
        auto dstSpan = std::span<ColorRGB8>{static_cast<ColorRGB8*>(static_cast<void*>(dst)), n};

        for (size_t i = 0; i < n; ++i) {
            uint32_t const aMulA = alpha * srcSpan[i].a;
            if (aMulA != 0U) {
                uint32_t const OneMin_aMulA = 65535 - aMulA;
                dstSpan[i].r = static_cast<uint8_t>(((aMulA * srcSpan[i].r) + (OneMin_aMulA * dstSpan[i].r)) >> 16);
                dstSpan[i].g = static_cast<uint8_t>(((aMulA * srcSpan[i].g) + (OneMin_aMulA * dstSpan[i].g)) >> 16);
                dstSpan[i].b = static_cast<uint8_t>(((aMulA * srcSpan[i].b) + (OneMin_aMulA * dstSpan[i].b)) >> 16);
            }
        }
    }

    void SDL_BlendRow_RGBA8_to_RGB565([[maybe_unused]] uint8_t const * src, uint8_t* dst, uint32_t alpha, size_t n)
    {
        auto srcSpan = std::span<ColorRGBA8 const>{static_cast<ColorRGBA8 const *>(static_cast<void const *>(src)), n};
        auto dstSpan = std::span<uint16_t>{static_cast<uint16_t*>(static_cast<void*>(dst)), n};

        for (size_t i = 0; i < n; ++i) {
            uint32_t const aMulA = (alpha * srcSpan[i].a) >> 8;
            if (aMulA != 0U) {
                uint32_t const OneMin_aMulA = 255 - aMulA;
                uint32_t const c            = dstSpan[i];
                dstSpan[i] = (((srcSpan[i].b * aMulA) + (((c & 0xF800) >> 8) * OneMin_aMulA)) & 0xF800) |
                             ((((srcSpan[i].g * aMulA) + (((c & 0x07E0) >> 3) * OneMin_aMulA)) >> 5) & 0x07E0) |
                             ((((srcSpan[i].r * aMulA) + (((c & 0x001F) << 3) * OneMin_aMulA)) >> 11) & 0x001F);
            }
        }
    }

    void SDL_BlendRow_RGBA4_to_RGB565([[maybe_unused]] uint8_t const * src, uint8_t* dst, uint32_t alpha, size_t n)
    {
        auto srcSpan = std::span<uint16_t const>{static_cast<uint16_t const *>(static_cast<void const *>(src)), n};
        auto dstSpan = std::span<uint16_t>{static_cast<uint16_t*>(static_cast<void*>(dst)), n};

        for (size_t i = 0; i < n; ++i) {
            uint32_t const c1 = dstSpan[i];
            uint32_t const c2 = srcSpan[i];

            uint32_t aMulA = c2 & 0xF;
            aMulA          = (alpha * aMulA) / 15; ///< upgrade to range 0-255
            if (aMulA != 0U) {
                uint32_t const OneMin_aMulA = 255 - aMulA;
                uint32_t result             = 0;
                result = ((((c2 & 0xF000) | 0x0800) * aMulA) + ((c1 & 0xF800) * OneMin_aMulA)) & 0xF80000;
                result |= (((((c2 & 0x0F00) >> 1) | 0x0040) * aMulA) + ((c1 & 0x07E0) * OneMin_aMulA)) & 0x07E000;
                result |= (((((c2 & 0x00F0) >> 3) | 0x0001) * aMulA) + ((c1 & 0x001F) * OneMin_aMulA)) & 0x001F00;
                /// multiplying by alpha resulted in shift.
                dstSpan[i] = static_cast<uint16_t>(result >> 8);
            }
        }
    }

} // namespace FIFE
