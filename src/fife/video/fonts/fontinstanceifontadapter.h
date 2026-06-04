// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_FONTS_FONTINSTANCEIFONTADAPTER_H
#define FIFE_VIDEO_FONTS_FONTINSTANCEIFONTADAPTER_H

#include <fifechan/font.hpp>

#include <memory>
#include <string>
#include <string_view>

#include "fontinstance.h"
#include "ifont.h"
#include "platform.h"
#include "textrenderpool.h"

namespace fcn
{
    class Graphics;
}

namespace FIFE
{

    class FIFE_API FontInstanceIFontAdapter final : public IFont, public fcn::Font
    {
        public:
            explicit FontInstanceIFontAdapter(std::shared_ptr<FontInstance> instance);
            ~FontInstanceIFontAdapter() override;

            // IFont
            void setRowSpacing(int32_t spacing) override;
            int32_t getRowSpacing() const override;
            void setGlyphSpacing(int32_t spacing) override;
            int32_t getGlyphSpacing() const override;
            void setAntiAlias(bool antiAlias) override;
            bool isAntiAlias() const override;
            void setBoldStyle(bool style) override;
            bool isBoldStyle() const override;
            void setItalicStyle(bool style) override;
            bool isItalicStyle() const override;
            void setUnderlineStyle(bool style) override;
            bool isUnderlineStyle() const override;
            void setStrikethroughStyle(bool style) override;
            bool isStrikethroughStyle() const override;
            void setDynamicColoring(bool coloring) override;
            bool isDynamicColoring() const override;

            int32_t getStringIndexAt(std::string const & text, int32_t x) const override;
            Image* getAsImage(std::string const & text) const override;
            Image* getAsImageMultiline(std::string const & text) const override;
            std::string splitTextToWidth(std::string const & text, int32_t render_width) override;
            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            SDL_Color getColor() const override;
            int32_t getWidth(std::string const & text) const override;
            int getHeight() const override;
            void invalidate() override;

            // fcn::Font
            int getWidth(std::string_view text) const override;
            int getStringIndexAt(std::string_view text, int x) const override;
            auto renderToSurface(std::string_view text) const
                -> std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter> override;

            void drawMultiLineString(fcn::Graphics* graphics, std::string const & text, int32_t x, int32_t y);

        private:
            SDL_Surface* renderString(std::string const & text) const;
            SDL_Surface* renderTrueType(std::string const & text) const;
            SDL_Surface* renderImage(std::string const & text) const;

            std::shared_ptr<FontInstance> m_instance;
            mutable TextRenderPool m_pool;
            SDL_Color m_color{255, 255, 255, 255};
            int32_t m_rowSpacing   = 0;
            int32_t m_glyphSpacing = 0;
            bool m_antiAlias       = true;
            bool m_bold            = false;
            bool m_italic          = false;
            bool m_dynamicColoring = false;
    };

} // namespace FIFE

#endif
