// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_FONTINSTANCE_H
#define FIFE_VIDEO_FONTS_FONTINSTANCE_H

#include <memory>
#include <string>

#include "fontface.h"
#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API FontInstance
    {
        public:
            FontInstance(
                std::shared_ptr<FontFace> face,
                int size,
                bool antialias   = true,
                bool bold        = false,
                bool italic      = false,
                int hinting      = 0,
                int glyphSpacing = 0,
                int rowSpacing   = 0);
            virtual ~FontInstance() = default;

            std::shared_ptr<FontFace> getFace() const
            {
                return m_face;
            }
            int getSize() const
            {
                return m_size;
            }
            bool isAntialias() const
            {
                return m_antialias;
            }
            bool isBold() const
            {
                return m_bold;
            }
            bool isItalic() const
            {
                return m_italic;
            }
            int getHinting() const
            {
                return m_hinting;
            }
            int getGlyphSpacing() const
            {
                return m_glyphSpacing;
            }
            int getRowSpacing() const
            {
                return m_rowSpacing;
            }

            FontInstanceKey getKey() const;

            virtual int getHeight() const                        = 0;
            virtual int getWidth(std::string const & text) const = 0;

        protected:
            std::shared_ptr<FontFace> m_face;
            int m_size;
            bool m_antialias;
            bool m_bold;
            bool m_italic;
            int m_hinting;
            int m_glyphSpacing;
            int m_rowSpacing;
    };

    class FIFE_API TrueTypeFontInstance final : public FontInstance
    {
        public:
            using FontInstance::FontInstance;
            int getHeight() const override;
            int getWidth(std::string const & text) const override;
    };

    class FIFE_API ImageFontInstance final : public FontInstance
    {
        public:
            using FontInstance::FontInstance;
            int getHeight() const override;
            int getWidth(std::string const & text) const override;
    };

} // namespace FIFE

#endif
