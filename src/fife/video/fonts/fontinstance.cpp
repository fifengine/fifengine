// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontinstance.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "imagefontface.h"
#include "truetypefontface.h"

namespace FIFE
{

    FontInstance::FontInstance(
        std::shared_ptr<FontFace> face,
        int size,
        bool antialias,
        bool bold,
        bool italic,
        int hinting,
        int glyphSpacing,
        int rowSpacing) :
        m_face(std::move(face)),
        m_size(size),
        m_antialias(antialias),
        m_bold(bold),
        m_italic(italic),
        m_hinting(hinting),
        m_glyphSpacing(glyphSpacing),
        m_rowSpacing(rowSpacing)
    {
        if (m_face == nullptr) {
            throw std::invalid_argument("FontFace must not be null");
        }
        if (size <= 0) {
            throw std::invalid_argument("Font size must be positive");
        }
    }

    FontInstanceKey FontInstance::getKey() const
    {
        assert("FontFace must still be alive" && m_face != nullptr);
        return FontInstanceKey{
            .asset     = m_face->getAssetHandle(),
            .size      = m_size,
            .bold      = m_bold,
            .italic    = m_italic,
            .antialias = m_antialias,
            .hinting   = m_hinting};
    }

    int TrueTypeFontInstance::getHeight() const
    {
        auto ttfFace = std::dynamic_pointer_cast<TrueTypeFontFace>(m_face);
        assert("Face must be TrueTypeFontFace" && ttfFace != nullptr);
        return TTF_GetFontHeight(ttfFace->getFont()) + m_rowSpacing;
    }

    int TrueTypeFontInstance::getWidth(std::string const & text) const
    {
        auto ttfFace = std::dynamic_pointer_cast<TrueTypeFontFace>(m_face);
        assert("Face must be TrueTypeFontFace" && ttfFace != nullptr);
        int w = 0;
        int h = 0;
        TTF_GetStringSize(ttfFace->getFont(), text.c_str(), text.length(), &w, &h);
        return w;
    }

    int ImageFontInstance::getHeight() const
    {
        auto imgFace = std::dynamic_pointer_cast<ImageFontFace>(m_face);
        assert("Face must be ImageFontFace" && imgFace != nullptr);
        return imgFace->getHeight() + m_rowSpacing;
    }

    int ImageFontInstance::getWidth(std::string const & text) const
    {
        auto imgFace = std::dynamic_pointer_cast<ImageFontFace>(m_face);
        assert("Face must be ImageFontFace" && imgFace != nullptr);
        int width = 0;
        for (auto const & c : text) {
            auto uc = static_cast<unsigned char>(c);
            width += imgFace->getGlyphWidth(static_cast<uint32_t>(uc)) + m_glyphSpacing;
        }
        return width > 0 ? width - m_glyphSpacing : 0;
    }

} // namespace FIFE
