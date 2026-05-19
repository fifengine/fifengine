// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_TRUETYPEFONT_H
#define FIFE_FONTS_TRUETYPEFONT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes
#include <SDL3_ttf/SDL_ttf.h>

// FIFE includes
#include "fontbase.h"

namespace FIFE
{
    class RawData;

    class FIFE_API TrueTypeFont;

    /**
     * SDL True Type Font implementation of Font. It uses the SDL_ttf library
     * to display True Type Fonts with SDL.
     *
     * NOTE: You must initialize the SDL_ttf library before using this
     *       class. Also, remember to call the SDL_ttf libraries quit
     *       function.
     *
     * Original author of this class is Walluce Pinkham. Some modifications
     * made by the Guichan team, and additonal modifications by the FIFE team.
     */
    class FIFE_API TrueTypeFont : public FontBase
    {
        public:
            /**
             * Constructor.
             *
             * @param filename the filename of the True Type Font.
             * @param size the size the font should be in.
             */
            TrueTypeFont(std::string const & filename, int32_t size);

            /**
             * Create a TrueTypeFont from raw font data loaded through VFS.
             *
             * @param data raw font file data (e.g. from VFS::open)
             * @param size the size the font should be in.
             * @param displayName a human-readable name for error messages
             */
            static TrueTypeFont* createFromRawData(RawData* data, int32_t size, std::string const & displayName = "");

            /**
             * Destructor.
             */
            ~TrueTypeFont() override;

            int32_t getWidth(std::string const & text) const override;

            int32_t getHeight() const override;

            void setBoldStyle(bool style) override;
            void setItalicStyle(bool style) override;
            void setUnderlineStyle(bool style) override;
            void setStrikethroughStyle(bool style) override;
            int32_t getFontStyleMask() const;

            SDL_Surface* renderString(std::string const & text) override;

            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;

        protected:
            TTF_Font* mFont;

            int32_t mFontStyle;

        private:
            TrueTypeFont(TTF_Font* font, std::string const & name, std::vector<uint8_t>&& fontData);

            std::vector<uint8_t> m_fontData;
    };
} // namespace FIFE

#endif //
