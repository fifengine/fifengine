// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "truetypefont.h"

// Standard C++ library includes
#include <cassert>
#include <string>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

#include <utf8cpp/utf8.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "vfs/raw/rawdata.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{

    TrueTypeFont::TrueTypeFont(std::string const & filename, int32_t size) :
        mFont(TTF_OpenFont(filename.c_str(), static_cast<float>(size))), mFontStyle(TTF_STYLE_NORMAL)
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

    TrueTypeFont* TrueTypeFont::createFromRawData(RawData* data, int32_t size, std::string const & displayName)
    {
        assert("font data must not be null" && data);
        std::vector<uint8_t> bytes = data->getDataInBytes();
        if (bytes.empty()) {
            throw FIFE::CannotOpenFile(displayName + " (empty font data)");
        }

        SDL_IOStream* stream = SDL_IOFromConstMem(bytes.data(), bytes.size());
        if (stream == nullptr) {
            throw FIFE::SDLException(std::string("SDL_IOFromConstMem failed: ") + SDL_GetError());
        }

        TTF_Font* font = TTF_OpenFontIO(stream, true, static_cast<float>(size));
        if (font == nullptr) {
            throw FIFE::CannotOpenFile(displayName + " (" + SDL_GetError() + ")");
        }

        return new TrueTypeFont(font, displayName, std::move(bytes));
    }

    TrueTypeFont::TrueTypeFont(TTF_Font* font, std::string const & name, std::vector<uint8_t>&& fontData) :
        mFont(font), mFontStyle(TTF_STYLE_NORMAL), m_fontData(std::move(fontData))
    {
        mFilename = name;

        mColor.r = mColor.g = mColor.b = mColor.a = 255;
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
            TTF_SetFontStyle(mFont, static_cast<TTF_FontStyleFlags>(mFontStyle));
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
            TTF_SetFontStyle(mFont, static_cast<TTF_FontStyleFlags>(mFontStyle));
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
            TTF_SetFontStyle(mFont, static_cast<TTF_FontStyleFlags>(mFontStyle));
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
            std::string err = "TTF_RenderText_";
            err += m_antiAlias ? "Blended" : "Solid";
            err += " failed for font '";
            err += mFilename;
            err += "' text '";
            err += text;
            err += "': ";
            err += SDL_GetError();
            throw FIFE::SDLException(err);
        }

        // Sanitize: Zero RGB for pixels where alpha=0 but RGB is non-zero.
        // FreeType's anti-aliasing can produce pixels with alpha=0 but non-zero
        // RGB (e.g., cyan 0,255,255). When alpha is lost in SDL3 texture
        // conversion, these become visible.
        if (SDL_ISPIXELFORMAT_ALPHA(SDL_GetPixelFormatDetails(renderedText->format)->format)) {
            if (SDL_LockSurface(renderedText)) {
                SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(renderedText->format);
                int32_t const pitch_px             = renderedText->pitch / 4;
                auto* pixels                       = static_cast<uint32_t*>(renderedText->pixels);
                for (int32_t y = 0; y < renderedText->h; ++y) {
                    uint32_t* row = pixels + (static_cast<size_t>(y) * static_cast<size_t>(pitch_px));
                    for (int32_t x = 0; x < renderedText->w; ++x) {
                        uint8_t r = 0;
                        uint8_t g = 0;
                        uint8_t b = 0;
                        uint8_t a = 0;
                        SDL_GetRGBA(row[x], fmt, nullptr, &r, &g, &b, &a);
                        // Fully transparent but has RGB
                        if (a == 0 && (r != 0 || g != 0 || b != 0)) {
                            row[x] = 0;
                        }
                    }
                }
                SDL_UnlockSurface(renderedText);
            }
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
