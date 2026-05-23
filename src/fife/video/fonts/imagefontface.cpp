// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "imagefontface.h"

#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <string>

namespace FIFE
{

    ImageFontFace::ImageFontFace(AssetHandle handle, std::string const & filepath, std::string const & glyphs) :
        FontFace(handle)
    {
        if (filepath.empty()) {
            throw std::invalid_argument("Image font file path must not be empty");
        }
        if (glyphs.empty()) {
            throw std::invalid_argument("Glyph string must not be empty");
        }

        m_sheet = IMG_Load(filepath.c_str());
        if (!m_sheet) {
            throw std::runtime_error("Failed to load glyph sheet: " + filepath);
        }

        m_height = m_sheet->h;
        extractGlyphs(m_sheet, glyphs);
    }

    ImageFontFace::~ImageFontFace()
    {
        for (auto& [codepoint, info] : m_glyphs) {
            if (info.surface)
                SDL_DestroySurface(info.surface);
        }
        if (m_sheet)
            SDL_DestroySurface(m_sheet);
    }

    bool ImageFontFace::supports(uint32_t codepoint) const
    {
        return m_glyphs.contains(codepoint);
    }

    SDL_Surface* ImageFontFace::getGlyphSurface(uint32_t codepoint) const
    {
        auto it = m_glyphs.find(codepoint);
        return it != m_glyphs.end() ? it->second.surface : nullptr;
    }

    int ImageFontFace::getGlyphWidth(uint32_t codepoint) const
    {
        auto it = m_glyphs.find(codepoint);
        assert("Glyph must have been extracted" && it != m_glyphs.end());
        return it != m_glyphs.end() ? it->second.width : 0;
    }

    void ImageFontFace::extractGlyphs(SDL_Surface* surface, std::string const & glyphs)
    {
        assert("Surface must not be null" && surface != nullptr);

        SDL_Surface* converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA8888);
        assert("Surface conversion failed" && converted != nullptr);

        uint32_t* pixels = static_cast<uint32_t*>(converted->pixels);
        int width        = converted->w;
        int pitch_px     = converted->pitch / 4;

        uint32_t separator = pixels[0];

        size_t x = 0;
        for (size_t i = 0; i < glyphs.size(); ++i) {
            uint32_t codepoint = static_cast<unsigned char>(glyphs[i]);

            while (x < static_cast<size_t>(width) && pixels[x] == separator) {
                ++x;
            }
            if (x >= static_cast<size_t>(width))
                break;

            size_t glyphStart = x;

            while (x < static_cast<size_t>(width) && pixels[x] != separator) {
                ++x;
            }
            size_t glyphEnd = x;

            int glyphWidth = static_cast<int>(glyphEnd - glyphStart);

            SDL_Surface* glyphSub = SDL_CreateSurface(glyphWidth, converted->h, SDL_PIXELFORMAT_RGBA8888);
            assert("Glyph sub-surface creation failed" && glyphSub != nullptr);

            uint32_t const * srcPixels = pixels;
            uint32_t* dstPixels        = static_cast<uint32_t*>(glyphSub->pixels);
            int dstPitchPx             = glyphSub->pitch / 4;

            for (int row = 0; row < converted->h; ++row) {
                for (int col = 0; col < glyphWidth; ++col) {
                    dstPixels[row * dstPitchPx + col] = srcPixels[row * pitch_px + glyphStart + col];
                }
            }

            m_glyphs[codepoint] = {glyphSub, glyphWidth};

            while (x < static_cast<size_t>(width) && pixels[x] == separator) {
                ++x;
            }
        }

        SDL_DestroySurface(converted);
    }

} // namespace FIFE
