// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "imagefontbase.h"

// Standard C++ library includes
#include <algorithm>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <utf8cpp/utf8.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{

    ImageFontBase::ImageFontBase() : m_placeholder(), m_height(0)
    {
    }

    ImageFontBase::~ImageFontBase()
    {
        auto i = m_glyphs.begin();
        for (; i != m_glyphs.end(); ++i) {
            SDL_DestroySurface(i->second.surface);
        }
    }

    int32_t ImageFontBase::getWidth(std::string const & text) const
    {
        int32_t w                           = 0;
        std::string::const_iterator text_it = text.begin();
        while (text_it != text.end()) {
            uint32_t const codepoint = utf8::next(text_it, text.end());
            auto it                  = m_glyphs.find(codepoint);

            if (it != m_glyphs.end()) {
                w += it->second.surface->w + getGlyphSpacing();
                continue;
            }

            if (m_placeholder.surface != nullptr) {
                w += m_placeholder.surface->w + getGlyphSpacing();
            }
        }
        return w;
    }

    int32_t ImageFontBase::getHeight() const
    {
        return m_height;
    }

    SDL_Surface* ImageFontBase::renderString(std::string const & text)
    {
        SDL_Surface* surface = SDL_CreateSurface(getWidth(text), getHeight(), SDL_PIXELFORMAT_RGBA8888);

        SDL_FillSurfaceRect(surface, nullptr, 0x00000000);

        SDL_Rect dst;
        dst.x = dst.y  = 0;
        s_glyph* glyph = nullptr;

        std::string::const_iterator text_it = text.begin();
        while (text_it != text.end()) {
            uint32_t const codepoint = utf8::next(text_it, text.end());
            auto it                  = m_glyphs.find(codepoint);

            if (it == m_glyphs.end()) {
                if (m_placeholder.surface == nullptr) {
                    continue;
                }
                glyph = &m_placeholder;
            } else {
                glyph = &(it->second);
            }
            dst.y = glyph->offset.y;
            dst.x += glyph->offset.x;

            SDL_BlitSurface(glyph->surface, nullptr, surface, &dst);
            dst.x += glyph->surface->w + getGlyphSpacing();
        }

        return surface;
    }

    void ImageFontBase::setColor(
        [[maybe_unused]] uint8_t r, [[maybe_unused]] uint8_t g, [[maybe_unused]] uint8_t b, [[maybe_unused]] uint8_t a)
    {
    }
} // namespace FIFE
