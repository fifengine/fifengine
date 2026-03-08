// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_TRUETYPEFONT_H
#define FIFE_FONTS_TRUETYPEFONT_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes
#include <SDL_ttf.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fontbase.h"

namespace FIFE
{

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
    class TrueTypeFont : public FontBase
    {
    public:
        /**
         * Constructor.
         *
         * @param filename the filename of the True Type Font.
         * @param size the size the font should be in.
         */
        TrueTypeFont(const std::string& filename, int32_t size);

        /**
         * Destructor.
         */
        virtual ~TrueTypeFont();

        // Inherited from Font

        virtual int32_t getWidth(const std::string& text) const;

        virtual int32_t getHeight() const;

        virtual void setBoldStyle(bool style);
        virtual void setItalicStyle(bool style);
        virtual void setUnderlineStyle(bool style);
        virtual void setStrikethroughStyle(bool style);
        virtual int32_t getFontStyleMask() const;

        virtual SDL_Surface* renderString(const std::string& text);

        virtual void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    protected:
        TTF_Font* mFont;

        int32_t mFontStyle;
    };
} // namespace FIFE

#endif //