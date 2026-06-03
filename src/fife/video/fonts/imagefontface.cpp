// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "imagefontface.h"

#include <SDL3_image/SDL_image.h>

#include <algorithm>
#include <cassert>
#include <span>
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
        if (m_sheet == nullptr) {
            throw std::runtime_error("Failed to load glyph sheet: " + filepath);
        }

        m_height = m_sheet->h;
        extractGlyphs(m_sheet, glyphs);
    }

    ImageFontFace::~ImageFontFace()
    {
        for (auto& [codepoint, info] : m_glyphs) {
            if (info.surface != nullptr) {
                SDL_DestroySurface(info.surface);
            }
        }
        if (m_sheet != nullptr) {
            SDL_DestroySurface(m_sheet);
        }
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

        size_t const width = static_cast<size_t>(converted->w);
        int const pitch_px = converted->pitch / 4;
        auto const pixels  = std::span(
            static_cast<uint32_t const *>(converted->pixels),
            static_cast<size_t>(pitch_px) * static_cast<size_t>(converted->h));

        uint32_t const separator = pixels[0];

        size_t x = 0;
        for (auto c : glyphs) {
            uint32_t const codepoint = static_cast<unsigned char>(c);

            while (x < width && pixels[x] == separator) {
                ++x;
            }
            if (x >= width) {
                break;
            }

            size_t const glyphStart = x;

            while (x < width && pixels[x] != separator) {
                ++x;
            }
            size_t const glyphEnd = x;

            int const glyphWidth = static_cast<int>(glyphEnd - glyphStart);

            SDL_Surface* glyphSub = SDL_CreateSurface(glyphWidth, converted->h, SDL_PIXELFORMAT_RGBA8888);
            assert("Glyph sub-surface creation failed" && glyphSub != nullptr);

            size_t const dstPitchPx = static_cast<size_t>(glyphSub->pitch) / 4U;
            auto dstPixels          = std::span(
                static_cast<uint32_t*>(glyphSub->pixels),
                static_cast<size_t>(dstPitchPx) * static_cast<size_t>(glyphSub->h));

            for (int row = 0; row < converted->h; ++row) {
                for (int col = 0; col < glyphWidth; ++col) {
                    dstPixels[static_cast<size_t>(row) * static_cast<size_t>(dstPitchPx) + static_cast<size_t>(col)] =
                        pixels
                            [static_cast<size_t>(row) * static_cast<size_t>(pitch_px) +
                             static_cast<size_t>(glyphStart) + static_cast<size_t>(col)];
                }
            }

            m_glyphs[codepoint] = GlyphInfo{.surface = glyphSub, .width = glyphWidth};

            while (x < width && pixels[x] == separator) {
                ++x;
            }
        }

        SDL_DestroySurface(converted);
    }

} // namespace FIFE
