// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontinstanceifontadapter.h"

#include <SDL3/SDL.h>

#include <fifechan.hpp>

#include <algorithm>
#include <cassert>
#include <list>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "imagefontface.h"
#include "truetypefontface.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include <utf8cpp/utf8.h>

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_GUI);
            return log;
        }

        bool shouldLogGuiText(std::string const & text)
        {
            return text == "Text 2" || text == "Text 3" || text == "Credits";
        }
    } // namespace

    FontInstanceIFontAdapter::FontInstanceIFontAdapter(std::shared_ptr<FontInstance> instance) :
        m_instance(std::move(instance))
    {
        assert("FontInstance must not be null" && m_instance);
    }

    FontInstanceIFontAdapter::~FontInstanceIFontAdapter() = default;

    void FontInstanceIFontAdapter::setRowSpacing(int32_t spacing)
    {
        m_rowSpacing = spacing;
    }

    int32_t FontInstanceIFontAdapter::getRowSpacing() const
    {
        return m_rowSpacing;
    }

    void FontInstanceIFontAdapter::setGlyphSpacing(int32_t spacing)
    {
        m_glyphSpacing = spacing;
    }

    int32_t FontInstanceIFontAdapter::getGlyphSpacing() const
    {
        return m_glyphSpacing;
    }

    void FontInstanceIFontAdapter::setAntiAlias(bool antiAlias)
    {
        m_antiAlias = antiAlias;
    }

    bool FontInstanceIFontAdapter::isAntiAlias() const
    {
        return m_antiAlias;
    }

    void FontInstanceIFontAdapter::setBoldStyle(bool style)
    {
        m_bold = style;
    }

    bool FontInstanceIFontAdapter::isBoldStyle() const
    {
        return m_bold;
    }

    void FontInstanceIFontAdapter::setItalicStyle(bool style)
    {
        m_italic = style;
    }

    bool FontInstanceIFontAdapter::isItalicStyle() const
    {
        return m_italic;
    }

    void FontInstanceIFontAdapter::setUnderlineStyle([[maybe_unused]] bool style)
    {
    }

    bool FontInstanceIFontAdapter::isUnderlineStyle() const
    {
        return false;
    }

    void FontInstanceIFontAdapter::setStrikethroughStyle([[maybe_unused]] bool style)
    {
    }

    bool FontInstanceIFontAdapter::isStrikethroughStyle() const
    {
        return false;
    }

    void FontInstanceIFontAdapter::setDynamicColoring(bool coloring)
    {
        m_dynamicColoring = coloring;
    }

    bool FontInstanceIFontAdapter::isDynamicColoring() const
    {
        return m_dynamicColoring;
    }

    void FontInstanceIFontAdapter::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        m_color.r = r;
        m_color.g = g;
        m_color.b = b;
        m_color.a = a;
    }

    SDL_Color FontInstanceIFontAdapter::getColor() const
    {
        return m_color;
    }

    int32_t FontInstanceIFontAdapter::getWidth(std::string const & text) const
    {
        return m_instance->getWidth(text);
    }

    int FontInstanceIFontAdapter::getHeight() const
    {
        return m_instance->getHeight() + m_rowSpacing;
    }

    void FontInstanceIFontAdapter::invalidate()
    {
        m_pool.invalidateCachedText();
    }

    SDL_Surface* FontInstanceIFontAdapter::renderTrueType(std::string const & text) const
    {
        auto ttfFace = std::dynamic_pointer_cast<TrueTypeFontFace>(m_instance->getFace());
        if (!ttfFace) {
            return nullptr;
        }

        if (text.empty()) {
            SDL_Surface* surface = SDL_CreateSurface(1, getHeight(), SDL_PIXELFORMAT_RGBA8888);
            SDL_FillSurfaceRect(surface, nullptr, 0x00000000);
            return surface;
        }

        TTF_Font* font = ttfFace->getFont();

        SDL_Surface* renderedText = nullptr;
        if (m_antiAlias) {
            renderedText = TTF_RenderText_Blended(font, text.c_str(), text.length(), m_color);
        } else {
            renderedText = TTF_RenderText_Solid(font, text.c_str(), text.length(), m_color);
        }
        if (renderedText == nullptr && !m_antiAlias) {
            renderedText = TTF_RenderText_Blended(font, text.c_str(), text.length(), m_color);
        }
        if (renderedText == nullptr) {
            throw SDLException(std::string("TTF_RenderText_ failed: ") + SDL_GetError());
        }

        /*FL_WARN(
            _log(),
            std::format(
                "renderTrueType result: text='{}' fmt={:#x} w={} h={} pitch={}",
                text,
                static_cast<unsigned>(renderedText->format),
                renderedText->w,
                renderedText->h,
                renderedText->pitch));*/

        if (renderedText->w > 0 && renderedText->h > 0) {
            uint32_t nonZero = 0;
            auto const px    = std::span(
                static_cast<uint32_t const *>(renderedText->pixels),
                static_cast<size_t>((renderedText->pitch / 4) * renderedText->h));
            auto const totalPx = px.size();
            for (size_t i = 0; i < totalPx && i < 10000; ++i) {
                if (*(px.data() + i) != 0) {
                    ++nonZero;
                }
            }

            /*FL_WARN(
                _log(),
                std::format(
                    "  first={:#010x} center={:#010x} nonZero/{}k={}",
                    *(px.data() + 0),
                    // NOLINTNEXTLINE(bugprone-misplaced-widening-cast)
                    *(px.data() +
                      static_cast<size_t>(((renderedText->h / 2) * (renderedText->pitch / 4)) + (renderedText->w / 2))),
                    totalPx > 10000 ? 10 : totalPx / 1000,
                    nonZero));*/
        }

        // NOLINTNEXTLINE(readability-simplify-boolean-expr)
        if (SDL_ISPIXELFORMAT_ALPHA(SDL_GetPixelFormatDetails(renderedText->format)->format)) {
            if (SDL_LockSurface(renderedText)) {
                int fixedCount                     = 0;
                SDL_PixelFormatDetails const * fmt = SDL_GetPixelFormatDetails(renderedText->format);
                int32_t const pitch_px             = renderedText->pitch / 4;
                auto pixels                        = std::span(
                    static_cast<uint32_t*>(renderedText->pixels),
                    static_cast<size_t>(pitch_px) * static_cast<size_t>(renderedText->h));
                for (int32_t y = 0; y < renderedText->h; ++y) {
                    auto row = pixels.subspan(
                        static_cast<size_t>(y) * static_cast<size_t>(pitch_px), static_cast<size_t>(renderedText->w));
                    for (int32_t x = 0; x < renderedText->w; ++x) {
                        uint8_t r = 0;
                        uint8_t g = 0;
                        uint8_t b = 0;
                        uint8_t a = 0;
                        SDL_GetRGBA(*(row.data() + static_cast<size_t>(x)), fmt, nullptr, &r, &g, &b, &a);
                        if (a == 0 && (r != 0 || g != 0 || b != 0)) {
                            *(row.data() + static_cast<size_t>(x)) = 0;
                            ++fixedCount;
                        }
                    }
                }
                SDL_UnlockSurface(renderedText);
                if (fixedCount > 0) {
                    FL_WARN(
                        _log(),
                        std::format("  alpha-fix: zeroed {} pixels with non-zero RGB but zero alpha", fixedCount));
                }
            }
        }

        return renderedText;
    }

    SDL_Surface* FontInstanceIFontAdapter::renderImage(std::string const & text) const
    {
        auto imgFace = std::dynamic_pointer_cast<ImageFontFace>(m_instance->getFace());
        if (!imgFace) {
            return nullptr;
        }

        SDL_Surface* surface = SDL_CreateSurface(getWidth(text), getHeight(), SDL_PIXELFORMAT_RGBA8888);
        SDL_FillSurfaceRect(surface, nullptr, 0x00000000);

        int32_t x                      = 0;
        std::string::const_iterator it = text.begin();
        while (it != text.end()) {
            uint32_t const codepoint = utf8::next(it, text.end());
            SDL_Surface* glyph       = imgFace->getGlyphSurface(codepoint);
            if (glyph == nullptr) {
                continue;
            }
            SDL_Rect const dst{.x = x, .y = 0, .w = glyph->w, .h = glyph->h};
            SDL_BlitSurface(glyph, nullptr, surface, &dst);
            x += glyph->w + m_glyphSpacing;
        }

        return surface;
    }

    SDL_Surface* FontInstanceIFontAdapter::renderString(std::string const & text) const
    {
        auto face = m_instance->getFace();
        if (std::dynamic_pointer_cast<TrueTypeFontFace>(face)) {
            return renderTrueType(text);
        }
        if (std::dynamic_pointer_cast<ImageFontFace>(face)) {
            return renderImage(text);
        }
        return nullptr;
    }

    Image* FontInstanceIFontAdapter::getAsImage(std::string const & text) const
    {
        Image* image = m_pool.getRenderedText(this, text);
        if (image == nullptr) {
            SDL_Surface* textSurface = renderString(text);
            auto newImage            = RenderBackend::instance()->createImage(textSurface);
            image                    = newImage.get();
            m_pool.addRenderedText(this, text, newImage.release());
        }
        return image;
    }

    Image* FontInstanceIFontAdapter::getAsImageMultiline(std::string const & text) const
    {
        Image* image = m_pool.getRenderedText(this, text);
        if (image != nullptr) {
            return image;
        }

        constexpr uint32_t newline_codepoint = '\n';

        std::vector<SDL_Surface*> lines;
        lines.reserve(16);
        int32_t render_width = 0;

        using Utf8Iter = utf8::iterator<std::string::const_iterator>;
        Utf8Iter it(text.begin(), text.begin(), text.end());
        Utf8Iter const end(text.end(), text.begin(), text.end());

        while (it != end) {
            std::string line;
            line.reserve(64);
            while (it != end && *it != newline_codepoint) {
                utf8::append(*it, std::back_inserter(line));
                ++it;
            }
            if (it != end && *it == newline_codepoint) {
                ++it;
            }
            SDL_Surface* text_surface = renderString(line);
            if (text_surface == nullptr) {
                continue;
            }
            render_width = std::max(render_width, text_surface->w);
            lines.emplace_back(text_surface);
        }

        int32_t const row_height    = m_rowSpacing + getHeight();
        int32_t const render_height = row_height * static_cast<int32_t>(lines.size());

        SDL_Surface* final_surface = SDL_CreateSurface(render_width, render_height, SDL_PIXELFORMAT_RGBA8888);
        if (final_surface == nullptr) {
            for (auto* surf : lines) {
                SDL_DestroySurface(surf);
            }
            throw SDLException(std::string("CreateRGBSurface failed: ") + SDL_GetError());
        }

        SDL_FillSurfaceRect(final_surface, nullptr, 0x00000000);

        int32_t ypos = 0;
        for (auto* line_surf : lines) {
            SDL_Rect const dst_rect{.x = 0, .y = ypos, .w = line_surf->w, .h = line_surf->h};
            SDL_SetSurfaceBlendMode(line_surf, SDL_BLENDMODE_NONE);
            SDL_BlitSurface(line_surf, nullptr, final_surface, &dst_rect);
            ypos += row_height;
            SDL_DestroySurface(line_surf);
        }

        auto newImage = RenderBackend::instance()->createImage(final_surface);
        image         = newImage.get();
        m_pool.addRenderedText(this, text, newImage.release());
        return image;
    }

    int32_t FontInstanceIFontAdapter::getStringIndexAt(std::string const & text, int32_t x) const
    {
        if (text.empty() || x <= 0) {
            return 0;
        }

        std::string::const_iterator cur = text.begin();
        utf8::next(cur, text.end());

        while (cur != text.end()) {
            std::string const buff(text.begin(), cur);
            if (getWidth(buff) > x) {
                return static_cast<int32_t>(buff.size());
            }
            utf8::next(cur, text.end());
        }

        if (x > getWidth(text)) {
            return static_cast<int32_t>(text.size());
        }
        return 0;
    }

    int FontInstanceIFontAdapter::getWidth(std::string_view text) const
    {
        return getWidth(std::string(text));
    }

    int FontInstanceIFontAdapter::getStringIndexAt(std::string_view text, int x) const
    {
        return getStringIndexAt(std::string(text), static_cast<int32_t>(x));
    }

    auto FontInstanceIFontAdapter::renderToSurface(std::string_view text) const
        -> std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter>
    {
        if (text.empty()) {
            return nullptr;
        }
        Image* image = getAsImage(std::string(text));
        if (image == nullptr) {
            FL_WARN(_log(), std::format("renderToSurface: getAsImage returned nullptr for '{}'", text));
            return nullptr;
        }
        if (image->getSurface() == nullptr) {
            FL_WARN(_log(), std::format("renderToSurface: image surface is nullptr for '{}'", text));
            return nullptr;
        }

        /*FL_WARN(
            _log(),
            std::format(
                "renderToSurface: text='{}' img=({}x{}) surf_fmt={:#x} surf_w={} surf_h={}",
                text,
                image->getWidth(),
                image->getHeight(),
                static_cast<unsigned>(image->getSurface()->format),
                image->getSurface()->w,
                image->getSurface()->h));*/

        SDL_Surface* src  = image->getSurface();
        SDL_Surface* copy = SDL_DuplicateSurface(src);
        if (copy == nullptr) {
            FL_WARN(_log(), std::format("renderToSurface: SDL_DuplicateSurface returned nullptr for '{}'", text));
            return nullptr;
        }

        /*FL_WARN(
            _log(),
            std::format(
                "renderToSurface: copy w={} h={} fmt={:#x}", copy->w, copy->h, static_cast<unsigned>(copy->format)));*/

        return std::unique_ptr<SDL_Surface, fcn::Font::SDL_SurfaceDeleter>(copy);
    }

    void FontInstanceIFontAdapter::drawMultiLineString(
        fcn::Graphics* graphics, std::string const & text, int32_t x, int32_t y)
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

        if (shouldLogGuiText(text)) {
            /*FL_LOG(
                _log(),
                std::format(
                    "drawMultiLineString: text='{}' input=({}, {}) yoffset={} clip=(x={}, y={}, w={}, h={}, offX={}, "
                    "offY={}) rect=({},{} {}x{}) image={}x{} surface={} fmt={:#x}",
                    text,
                    x,
                    y,
                    yoffset,
                    clip.x,
                    clip.y,
                    clip.width,
                    clip.height,
                    clip.xOffset,
                    clip.yOffset,
                    rect.x,
                    rect.y,
                    rect.w,
                    rect.h,
                    image->getWidth(),
                    image->getHeight(),
                    surface != nullptr ? std::format("{}x{}", surface->w, surface->h) : std::string("null"),
                    surface != nullptr ? static_cast<unsigned>(surface->format) : 0U));*/
        }

        if (!rect.intersects(Rect(clip.x, clip.y, clip.width, clip.height))) {
            return;
        }
        image->render(rect);
    }

    std::string FontInstanceIFontAdapter::splitTextToWidth(std::string const & text, int32_t render_width)
    {
        uint32_t const whitespace = ' ';
        if (render_width <= 0 || text.empty()) {
            return text;
        }
        std::string output;
        std::string line;
        std::string::const_iterator pos = text.begin();
        std::list<std::pair<size_t, std::string::const_iterator>> break_pos;
        bool firstLine = true;

        while (pos != text.end()) {
            break_pos.clear();
            if (!firstLine) {
                line = "\n";
            } else {
                firstLine = false;
            }

            while (getWidth(line) < render_width && pos != text.end()) {
                uint32_t const codepoint = utf8::next(pos, text.end());
                if (codepoint == whitespace && !line.empty()) {
                    break_pos.emplace_back(line.length(), pos);
                }
                if (codepoint != '\n') {
                    utf8::append(codepoint, back_inserter(line));
                } else {
                    output.append(line);
                    line = "";
                    break;
                }
            }

            if (pos == text.end()) {
                break;
            }

            if (break_pos.empty()) {
                if (utf8::distance(line.begin(), line.end()) <= 1 && line != "\n") {
                    output.append(line);
                    continue;
                }
                if (line == "\n") {
                    ++pos;
                }
            } else {
                line.resize(break_pos.back().first);
                pos = break_pos.back().second;
            }
            output.append(line);
        }
        if (!line.empty()) {
            output.append(line);
        }
        return output;
    }

} // namespace FIFE
