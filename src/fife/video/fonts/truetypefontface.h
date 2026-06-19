// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_TRUETYPEFONTFACE_H
#define FIFE_VIDEO_FONTS_TRUETYPEFONTFACE_H

#include <SDL3_ttf/SDL_ttf.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "fontface.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API TrueTypeFontFace final : public FontFace
    {
        public:
            TrueTypeFontFace(AssetHandle handle, std::string const & filepath, int ptsize);
            TrueTypeFontFace(
                AssetHandle handle, uint8_t const * data, size_t size, int ptsize, std::string const & name);
            ~TrueTypeFontFace() override;

            bool supports(uint32_t codepoint) const override;
            TTF_Font* getFont() const
            {
                return m_font;
            }
            int getPointSize() const override
            {
                return m_ptsize;
            }

            void setDPIScale(float factor) override;
            float getDPIScale() const override
            {
                return m_dpiScale;
            }

        private:
            float m_dpiScale = 1.0F;
            void initCoverage();
            void reloadFont();

            TTF_Font* m_font = nullptr;
            int m_ptsize     = 0;
            std::string m_filepath;
            std::vector<uint8_t> m_fontData;
            std::vector<std::pair<uint32_t, uint32_t>> m_coverage;
    };

} // namespace FIFE

#endif
