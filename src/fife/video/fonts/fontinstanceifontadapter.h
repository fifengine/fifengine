// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>
#include <string>

#include "fontinstance.h"
#include "ifont.h"
#include "platform.h"
#include "textrenderpool.h"

namespace FIFE
{

    class FIFE_API FontInstanceIFontAdapter final : public IFont
    {
        public:
            explicit FontInstanceIFontAdapter(std::shared_ptr<FontInstance> instance);
            ~FontInstanceIFontAdapter() override;

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
            Image* getAsImage(std::string const & text) override;
            Image* getAsImageMultiline(std::string const & text) override;
            std::string splitTextToWidth(std::string const & text, int32_t render_width) override;
            void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) override;
            SDL_Color getColor() const override;
            int32_t getWidth(std::string const & text) const override;
            int32_t getHeight() const override;
            void invalidate() override;

        private:
            SDL_Surface* renderString(std::string const & text);
            SDL_Surface* renderTrueType(std::string const & text);
            SDL_Surface* renderImage(std::string const & text);

            std::shared_ptr<FontInstance> m_instance;
            TextRenderPool m_pool;
            SDL_Color m_color{255, 255, 255, 255};
            int32_t m_rowSpacing   = 0;
            int32_t m_glyphSpacing = 0;
            bool m_antiAlias       = true;
            bool m_bold            = false;
            bool m_italic          = false;
            bool m_dynamicColoring = false;
    };

} // namespace FIFE
