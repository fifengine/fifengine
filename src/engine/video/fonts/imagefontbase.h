// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_IMAGEFONTBASE_H
#define FIFE_FONTS_IMAGEFONTBASE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
#include "fontbase.h"
#include "util/structures/point.h"

namespace FIFE
{

    /** ImageFont base class
     *
     *  Just set the glyphs/placeholder in any derived class and the rendering
     *  is handled by this class. Also frees all glyph surfaces on destruction.
     */
    class /*FIFE_API*/ ImageFontBase : public FontBase
    {
        public:
            /**
             * Constructor.
             */
            ImageFontBase();

            /**
             * Destructor.
             */
            ~ImageFontBase() override;

            int32_t getWidth(std::string const & text) const override;

            int32_t getHeight() const override;

            SDL_Surface* renderString(std::string const & text) override;
            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;

        protected:
            // A glyph (visible character)
            struct /*FIFE_API*/ s_glyph
            {
                    // The offset of the glyph relative to the top-left corner.
                    Point offset;
                    // The glyphs image
                    // should be with SDL_SRCALPHA off, so that it's just copied over.
                    SDL_Surface* surface;
            };

            using type_glyphs = std::map<int32_t, s_glyph>;
            type_glyphs m_glyphs;

            // The glyph used, when the real glyph is not found
            // Should default to '?'
            s_glyph m_placeholder;

            int32_t m_height;
    };
} // namespace FIFE

#endif // end GCN_SDLTRUETYPEFONT_HPP
