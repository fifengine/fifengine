// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_IMAGEFONTFACE_H
#define FIFE_VIDEO_FONTS_IMAGEFONTFACE_H

#include <SDL3/SDL_surface.h>

#include <cstdint>
#include <map>
#include <string>

#include "fontface.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API ImageFontFace final : public FontFace
    {
        public:
            ImageFontFace(AssetHandle handle, std::string const & filepath, std::string const & glyphs);
            ~ImageFontFace() override;

            bool supports(uint32_t codepoint) const override;
            int getPointSize() const override
            {
                return m_height;
            }
            SDL_Surface* getGlyphSurface(uint32_t codepoint) const;
            int getGlyphWidth(uint32_t codepoint) const;
            int getHeight() const
            {
                return m_height;
            }

            void setDPIScale(float factor) override;
            float getDPIScale() const override
            {
                return m_dpiScale;
            }

        private:
            float m_dpiScale = 1.0f;
            void extractGlyphs(SDL_Surface* surface, std::string const & glyphs);

            SDL_Surface* m_sheet = nullptr;
            int m_height         = 0;
            int m_originalHeight = 0;
            std::string m_glyphsString;
            struct GlyphInfo
            {
                    SDL_Surface* surface;
                    int width;
            };
            std::map<uint32_t, GlyphInfo> m_glyphs;
    };

} // namespace FIFE

#endif
