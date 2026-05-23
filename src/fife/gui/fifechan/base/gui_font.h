// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GUI_FONT_H
#define FIFE_GUI_FONT_H

#include <SDL3/SDL.h>

#include <fifechan/font.hpp>

#include <memory>
#include <string>
#include <string_view>

#include "platform.h"
#include "video/fonts/ifont.h"

namespace FIFE
{

    class FIFE_API GuiFont : public fcn::Font
    {
        public:
            explicit GuiFont(IFont* font);
            ~GuiFont() override;

            int getWidth(std::string_view text) const override;
            int getHeight() const override;
            int getStringIndexAt(std::string_view text, int x) const override;
            auto renderToSurface(std::string_view text) const
                -> std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter> override;

            void drawMultiLineString(fcn::Graphics* graphics, std::string const & text, int32_t x, int32_t y);
            std::string splitTextToWidth(std::string const & text, int32_t render_width);
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
            Image* getAsImage(std::string const & text);
            Image* getAsImageMultiline(std::string const & text);
            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
            SDL_Color getColor() const;
            void invalidate();

        private:
            IFont* m_font;
    };
} // namespace FIFE

#endif
