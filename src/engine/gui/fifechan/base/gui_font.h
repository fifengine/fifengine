// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_FONT_H
#define FIFE_GUI_FONT_H

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan/font.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/fonts/ifont.h"

namespace FIFE
{

    class GuiFont : public fcn::Font, public IFont
    {
    public:
        /** Constructor
         *  Takes the ownership of given font
         */
        GuiFont(IFont* font);
        virtual ~GuiFont();

        int32_t getStringIndexAt(const std::string& text, int32_t x) const;
        void drawString(fcn::Graphics* graphics, const std::string& text, int32_t x, int32_t y);
        void drawMultiLineString(fcn::Graphics* graphics, const std::string& text, int32_t x, int32_t y);
        std::string splitTextToWidth(const std::string& text, int32_t render_width);

        void setRowSpacing(int32_t spacing);
        int32_t getRowSpacing() const;
        void setGlyphSpacing(int32_t spacing);
        int32_t getGlyphSpacing() const;
        void setAntiAlias(bool antiAlias);
        bool isAntiAlias() const;
        void setBoldStyle(bool style);
        bool isBoldStyle() const;
        void setItalicStyle(bool style);
        bool isItalicStyle() const;
        void setUnderlineStyle(bool style);
        bool isUnderlineStyle() const;
        void setStrikethroughStyle(bool style);
        bool isStrikethroughStyle() const;
        void setDynamicColoring(bool coloring);
        bool isDynamicColoring() const;
        Image* getAsImage(const std::string& text);
        Image* getAsImageMultiline(const std::string& text);
        void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        SDL_Color getColor() const;
        int32_t getWidth(const std::string& text) const;
        int32_t getHeight() const;
        void invalidate();

    private:
        IFont* m_font;
    };
} // namespace FIFE

#endif
