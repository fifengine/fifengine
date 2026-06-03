// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "truetypefontface.h"

#include <algorithm>
#include <cassert>
#include <span>
#include <stdexcept>
#include <string>

namespace FIFE
{

    TrueTypeFontFace::TrueTypeFontFace(AssetHandle handle, std::string const & filepath, int ptsize) :
        FontFace(handle), m_ptsize(ptsize)
    {
        if (filepath.empty()) {
            throw std::invalid_argument("TrueType file path must not be empty");
        }
        if (ptsize <= 0) {
            throw std::invalid_argument("Font size must be positive");
        }
        m_font = TTF_OpenFont(filepath.c_str(), static_cast<float>(ptsize));
        if (m_font == nullptr) {
            throw std::runtime_error("Failed to open TrueType font: " + filepath);
        }
        initCoverage();
    }

    TrueTypeFontFace::TrueTypeFontFace(
        AssetHandle handle, uint8_t const * data, size_t size, int ptsize, std::string const & /*name*/) :
        FontFace(handle), m_ptsize(ptsize)
    {
        if (data == nullptr) {
            throw std::invalid_argument("TrueType font data must not be null");
        }
        if (size == 0) {
            throw std::invalid_argument("TrueType font data size must be positive");
        }
        if (ptsize <= 0) {
            throw std::invalid_argument("Font size must be positive");
        }
        auto const fontSpan = std::span(data, size);
        m_fontData.assign(fontSpan.begin(), fontSpan.end());
        SDL_IOStream* iostream = SDL_IOFromConstMem(m_fontData.data(), m_fontData.size());
        if (iostream == nullptr) {
            throw std::runtime_error("Failed to create SDL_IOStream from font data");
        }
        m_font = TTF_OpenFontIO(iostream, true, static_cast<float>(ptsize));
        if (m_font == nullptr) {
            throw std::runtime_error("Failed to open TrueType font from memory");
        }
        initCoverage();
    }

    TrueTypeFontFace::~TrueTypeFontFace()
    {
        if (m_font != nullptr) {
            TTF_CloseFont(m_font);
        }
    }

    bool TrueTypeFontFace::supports(uint32_t codepoint) const
    {
        assert("TrueType font must be loaded" && m_font != nullptr);
        if (std::ranges::any_of(m_coverage, [&](auto const & range) {
                return codepoint >= range.first && codepoint <= range.second;
            })) {
            return true;
        }
        if (m_font != nullptr) {
            return TTF_FontHasGlyph(m_font, codepoint);
        }
        return false;
    }

    void TrueTypeFontFace::initCoverage()
    {
        assert("TrueType font must be loaded" && m_font != nullptr);
        if (m_font == nullptr) {
            return;
        }

        m_coverage.emplace_back(0x0020, 0x007E);
        m_coverage.emplace_back(0x00A0, 0x00FF);
    }

} // namespace FIFE
