// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "truetypefont.h"

// Standard C++ library includes
#include <cassert>
#include <string>

// 3rd party library includes
#include <SDL3/SDL.h>

#include <utf8.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{

    TrueTypeFont::TrueTypeFont(std::string const & filename, int32_t size) :
        mFont(TTF_OpenFont(filename.c_str(), size)), mFontStyle(TTF_STYLE_NORMAL)
    {
        mFilename = filename;

        if (mFont == nullptr) {
            throw FIFE::CannotOpenFile(filename + " (" + SDL_GetError() + ")");
        }

        mColor.r = mColor.g = mColor.b = mColor.a = 255;

        // Maybe we should add an setting for that
        // TTF_HINTING_NORMAL // default
        // TTF_HINTING_LIGHT
        // TTF_HINTING_MONO
        // TTF_HINTING_NONE
        // TTF_SetFontHinting(mFont, TTF_HINTING_LIGHT);
    }

    TrueTypeFont::~TrueTypeFont()
    {
        TTF_CloseFont(mFont);
    }

    int32_t TrueTypeFont::getWidth(std::string const & text) const
    {
        int32_t w = 0;
        int32_t h = 0;
        assert(utf8::is_valid(text.begin(), text.end()));
        TTF_GetStringSize(mFont, text.c_str(), text.length(), &w, &h);
        return w;
    }

    int32_t TrueTypeFont::getHeight() const
    {
        return TTF_GetFontHeight(mFont) + getRowSpacing();
    }

    void TrueTypeFont::setBoldStyle(bool style)
    {
        if (style != m_boldStyle) {
            if (style) {
                mFontStyle |= TTF_STYLE_BOLD;
            } else {
                mFontStyle &= ~TTF_STYLE_BOLD;
            }
            m_boldStyle = style;
            TTF_SetFontStyle(mFont, mFontStyle);
        }
    }

    void TrueTypeFont::setItalicStyle(bool style)
    {
        if (style != m_italicStyle) {
            if (style) {
                mFontStyle |= TTF_STYLE_ITALIC;
            } else {
                mFontStyle &= ~TTF_STYLE_ITALIC;
            }
            m_italicStyle = style;
            TTF_SetFontStyle(mFont, mFontStyle);
        }
    }

    void TrueTypeFont::setUnderlineStyle(bool style)
    {
        if (style != m_underlineStyle) {
            if (style) {
                mFontStyle |= TTF_STYLE_UNDERLINE;
            } else {
                mFontStyle &= ~TTF_STYLE_UNDERLINE;
            }
            m_underlineStyle = style;
            TTF_SetFontStyle(mFont, mFontStyle);
        }
    }

    void TrueTypeFont::setStrikethroughStyle(bool style)
    {
        // ToDo: It's currently not available.
        if (style != m_strikeStyle) {
            m_strikeStyle = style;
        }
    }

    int32_t TrueTypeFont::getFontStyleMask() const
    {
        return mFontStyle;
    }

    SDL_Surface* TrueTypeFont::renderString(std::string const & text)
    {
        if (text.empty()) {
            SDL_Surface* surface = SDL_CreateSurface(1, getHeight(), SDL_PIXELFORMAT_RGBA8888);
            SDL_FillSurfaceRect(surface, nullptr, 0x00000000);
            return surface;
        }

        SDL_Surface* renderedText = nullptr;
        if (m_antiAlias) {
            renderedText = TTF_RenderText_Blended(mFont, text.c_str(), text.length(), mColor);
        } else {
            renderedText = TTF_RenderText_Solid(mFont, text.c_str(), text.length(), mColor);
        }
        // Workaround for a freetype bug, see here:
        // http://www.nabble.com/SDL_ttf-and-DPMSDisable-bug-is-back-or-still-there-to9578884.html
        if (renderedText == nullptr && !m_antiAlias) {
            renderedText = TTF_RenderText_Blended(mFont, text.c_str(), text.length(), mColor);
        }
        // Still could not render? Something went horribly wrong!
        if (renderedText == nullptr) {
            throw FIFE::SDLException(SDL_GetError());
        }
        return renderedText;
    }

    void TrueTypeFont::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        mColor.r = r;
        mColor.g = g;
        mColor.b = b;
        mColor.a = a;
    }
} // namespace FIFE
