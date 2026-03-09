// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_FONTBASE_H
#define FIFE_FONTS_FONTBASE_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "ifont.h"
#include "textrenderpool.h"

struct SDL_Surface;
namespace FIFE
{

    /** Abstract Font Base Class
     *  Uses a pool for rendered strings.
     *  @see TextRenderPool
     */
    class FontBase : public IFont
    {
    public:
        FontBase();
        virtual ~FontBase() { }

        void invalidate();
        void setRowSpacing(int32_t spacing);
        int32_t getRowSpacing() const;
        void setGlyphSpacing(int32_t spacing);
        int32_t getGlyphSpacing() const;

        virtual void setAntiAlias(bool antiAlias);
        virtual bool isAntiAlias() const;
        virtual void setBoldStyle(bool style);
        virtual bool isBoldStyle() const;
        virtual void setItalicStyle(bool style);
        virtual bool isItalicStyle() const;
        virtual void setUnderlineStyle(bool style);
        virtual bool isUnderlineStyle() const;
        virtual void setStrikethroughStyle(bool style);
        virtual bool isStrikethroughStyle() const;

        virtual void setDynamicColoring(bool coloring);
        virtual bool isDynamicColoring() const;

        virtual int32_t getStringIndexAt(const std::string& text, int32_t x) const;

        Image* getAsImage(const std::string& text);
        Image* getAsImageMultiline(const std::string& text);
        std::string splitTextToWidth(const std::string& text, int32_t render_width);

        SDL_Color getColor() const;

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
