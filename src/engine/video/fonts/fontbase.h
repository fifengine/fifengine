// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_FONTBASE_H
#define FIFE_FONTS_FONTBASE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include "util/base/fife_stdint.h"
#include <string>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "ifont.h"
#include "textrenderpool.h"

struct SDL_Surface;
namespace FIFE
{

    /** Abstract Font Base Class
     *  Uses a pool for rendered strings.
     *  @see TextRenderPool
     */
    class /*FIFE_API*/ FontBase : public IFont
    {
    public:
        FontBase();
        ~FontBase() override;

        void invalidate() override;
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

        int32_t getStringIndexAt(const std::string& text, int32_t x) const override;

        Image* getAsImage(const std::string& text) override;
        Image* getAsImageMultiline(const std::string& text) override;
        std::string splitTextToWidth(const std::string& text, int32_t render_width) override;

        SDL_Color getColor() const override;

        virtual SDL_Surface* renderString(const std::string& text) = 0;

    protected:
        TextRenderPool m_pool;

        SDL_Color mColor;
        int32_t mGlyphSpacing;
        int32_t mRowSpacing;

        std::string mFilename;
        bool m_antiAlias;
        bool m_boldStyle;
        bool m_italicStyle;
        bool m_underlineStyle;
        bool m_strikeStyle;
        bool m_coloring;
    };

} // namespace FIFE

#endif
