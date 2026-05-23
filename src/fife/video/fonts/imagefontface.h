// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

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
            SDL_Surface* getGlyphSurface(uint32_t codepoint) const;
            int getGlyphWidth(uint32_t codepoint) const;
            int getHeight() const
            {
                return m_height;
            }

        private:
            void extractGlyphs(SDL_Surface* surface, std::string const & glyphs);

            SDL_Surface* m_sheet = nullptr;
            int m_height         = 0;
            struct GlyphInfo
            {
                    SDL_Surface* surface;
                    int width;
            };
            std::map<uint32_t, GlyphInfo> m_glyphs;
    };

} // namespace FIFE
