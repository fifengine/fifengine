// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <string>

// 3rd party library includes
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "imagefontbase.h"

namespace FIFE
{

    ImageFontBase::ImageFontBase() : m_height(0), m_placeholder() { }

    ImageFontBase::~ImageFontBase()
    {
        auto i = m_glyphs.begin();
        for (; i != m_glyphs.end(); ++i) {
            SDL_FreeSurface(i->second.surface);
        }
    }

    int32_t ImageFontBase::getWidth(const std::string& text) const
    {
        int32_t w                           = 0;
        std::string::const_iterator text_it = text.begin();
        while (text_it != text.end()) {
            const uint32_t codepoint = utf8::next(text_it, text.end());
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

    SDL_Surface* ImageFontBase::renderString(const std::string& text)
    {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, getWidth(text), getHeight(), 32, RMASK, GMASK, BMASK, AMASK);

        SDL_FillRect(surface, nullptr, 0x00000000);

        SDL_Rect dst;
        dst.x = dst.y  = 0;
        s_glyph* glyph = nullptr;

        std::string::const_iterator text_it = text.begin();
        while (text_it != text.end()) {
            const uint32_t codepoint = utf8::next(text_it, text.end());
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

    void ImageFontBase::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { }
} // namespace FIFE
