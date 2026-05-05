// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "gui_font.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes
#include <fifechan.hpp>

// FIFE includes
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

namespace FIFE
{
    GuiFont::GuiFont(IFont* font) : m_font(font)
    {
        assert(font);
    }

    GuiFont::~GuiFont()
    {
        delete m_font;
    }

    int32_t GuiFont::getStringIndexAt(std::string const & text, int32_t x) const
    {
        return m_font->getStringIndexAt(text, x);
    }

    void GuiFont::drawString(fcn::Graphics* graphics, std::string const & text, int32_t x, int32_t y)
    {
        if (text.empty()) {
            return;
        }

        int32_t const yoffset = getRowSpacing() / 2;

        fcn::ClipRectangle const & clip = graphics->getCurrentClipArea();
        FIFE::Rect rect;
        rect.x = x + clip.xOffset;
        rect.y = y + clip.yOffset + yoffset;
        rect.w = getWidth(text);
        rect.h = getHeight();

        if (!rect.intersects(Rect(clip.x, clip.y, clip.width, clip.height))) {
            return;
        }

        Image* image = nullptr;
        if (isDynamicColoring()) {
            SDL_Color const color = getColor();
            setColor(graphics->getColor().r, graphics->getColor().g, graphics->getColor().b, graphics->getColor().a);
            image = getAsImage(text);
            setColor(color.r, color.g, color.b, color.a);
        } else {
            image = getAsImage(text);
        }
        image->render(rect);
    }

    void GuiFont::drawMultiLineString(fcn::Graphics* graphics, std::string const & text, int32_t x, int32_t y)
    {
        if (text.empty()) {
            return;
        }

        int32_t const yoffset = getRowSpacing() / 2;

        fcn::ClipRectangle const & clip = graphics->getCurrentClipArea();

        Image* image = nullptr;
        if (isDynamicColoring()) {
            SDL_Color const color = getColor();
            setColor(graphics->getColor().r, graphics->getColor().g, graphics->getColor().b, graphics->getColor().a);
            image = getAsImageMultiline(text);
            setColor(color.r, color.g, color.b, color.a);
        } else {
            image = getAsImageMultiline(text);
        }

        FIFE::Rect rect;
        rect.x = x + clip.xOffset;
        rect.y = y + clip.yOffset + yoffset;
        rect.w = image->getWidth();
        rect.h = image->getHeight();
        if (!rect.intersects(Rect(clip.x, clip.y, clip.width, clip.height))) {
            return;
        }
        image->render(rect);
    }

    void GuiFont::setRowSpacing(int32_t spacing)
    {
        m_font->setRowSpacing(spacing);
    }

    int32_t GuiFont::getRowSpacing() const
    {
        return m_font->getRowSpacing();
    }

    void GuiFont::setGlyphSpacing(int32_t spacing)
    {
        m_font->setGlyphSpacing(spacing);
    }

    int32_t GuiFont::getGlyphSpacing() const
    {
        return m_font->getGlyphSpacing();
    }

    void GuiFont::setAntiAlias(bool antiAlias)
    {
        m_font->setAntiAlias(antiAlias);
    }

    bool GuiFont::isAntiAlias() const
    {
        return m_font->isAntiAlias();
    }

    void GuiFont::setBoldStyle(bool style)
    {
        m_font->setBoldStyle(style);
    }

    bool GuiFont::isBoldStyle() const
    {
        return m_font->isBoldStyle();
    }

    void GuiFont::setItalicStyle(bool style)
    {
        m_font->setItalicStyle(style);
    }

    bool GuiFont::isItalicStyle() const
    {
        return m_font->isItalicStyle();
    }

    void GuiFont::setUnderlineStyle(bool style)
    {
        m_font->setUnderlineStyle(style);
    }

    bool GuiFont::isUnderlineStyle() const
    {
        return m_font->isUnderlineStyle();
    }

    void GuiFont::setStrikethroughStyle(bool style)
    {
        m_font->setStrikethroughStyle(style);
    }

    bool GuiFont::isStrikethroughStyle() const
    {
        return m_font->isStrikethroughStyle();
    }

    void GuiFont::setDynamicColoring(bool coloring)
    {
        m_font->setDynamicColoring(coloring);
    }

    bool GuiFont::isDynamicColoring() const
    {
        return m_font->isDynamicColoring();
    }

    Image* GuiFont::getAsImage(std::string const & text)
    {
        return m_font->getAsImage(text);
    }

    Image* GuiFont::getAsImageMultiline(std::string const & text)
    {
        return m_font->getAsImageMultiline(text);
    }

    std::string GuiFont::splitTextToWidth(std::string const & text, int32_t render_width)
    {
        return m_font->splitTextToWidth(text, render_width);
    }

    void GuiFont::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_font->setColor(r, g, b, a);
    }

    SDL_Color GuiFont::getColor() const
    {
        return m_font->getColor();
    }

    int32_t GuiFont::getWidth(std::string const & text) const
    {
        return m_font->getWidth(text);
    }

    int32_t GuiFont::getHeight() const
    {
        return m_font->getHeight();
    }

    void GuiFont::invalidate()
    {
        m_font->invalidate();
    }
} // namespace FIFE
