// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "gui_font.h"

#include <fifechan.hpp>

#include <cassert>
#include <memory>
#include <string>

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

    int GuiFont::getWidth(std::string_view text) const
    {
        return m_font->getWidth(std::string(text));
    }

    int GuiFont::getHeight() const
    {
        return m_font->getHeight();
    }

    int GuiFont::getStringIndexAt(std::string_view text, int x) const
    {
        return m_font->getStringIndexAt(std::string(text), x);
    }

    auto GuiFont::renderToSurface(std::string_view text) const
        -> std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter>
    {
        if (text.empty()) {
            return nullptr;
        }
        Image* image = m_font->getAsImage(std::string(text));
        if (image == nullptr) {
            return nullptr;
        }
        SDL_Surface* src = image->getSurface();
        if (src == nullptr) {
            return nullptr;
        }
        SDL_Surface* copy = SDL_DuplicateSurface(src);
        return std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter>(copy);
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
        rect.w = static_cast<int32_t>(image->getWidth());
        rect.h = static_cast<int32_t>(image->getHeight());
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

    void GuiFont::setUnderlineStyle([[maybe_unused]] bool style)
    {
    }

    bool GuiFont::isUnderlineStyle() const
    {
        return false;
    }

    void GuiFont::setStrikethroughStyle([[maybe_unused]] bool style)
    {
    }

    bool GuiFont::isStrikethroughStyle() const
    {
        return false;
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

    void GuiFont::invalidate()
    {
        m_font->invalidate();
    }
} // namespace FIFE
