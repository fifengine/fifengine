// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <string>
#include <utility>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "fontbase.h"

namespace FIFE
{

    FontBase::FontBase() :

        mColor(),
        mGlyphSpacing(0),
        mRowSpacing(0),

        m_antiAlias(true),
        m_boldStyle(false),
        m_italicStyle(false),
        m_underlineStyle(false),
        m_strikeStyle(false),
        m_coloring(false)
    {
    }

    FontBase::~FontBase() = default;

    void FontBase::invalidate()
    {
        m_pool.invalidateCachedText();
    }

    void FontBase::setRowSpacing(int32_t spacing)
    {
        mRowSpacing = spacing;
    }

    int32_t FontBase::getRowSpacing() const
    {
        return mRowSpacing;
    }

    void FontBase::setGlyphSpacing(int32_t spacing)
    {
        mGlyphSpacing = spacing;
    }

    int32_t FontBase::getGlyphSpacing() const
    {
        return mGlyphSpacing;
    }

    void FontBase::setAntiAlias(bool antiAlias)
    {
        m_antiAlias = antiAlias;
    }

    bool FontBase::isAntiAlias() const
    {
        return m_antiAlias;
    }

    void FontBase::setBoldStyle(bool style)
    {
        m_boldStyle = style;
    }

    bool FontBase::isBoldStyle() const
    {
        return m_boldStyle;
    }

    void FontBase::setItalicStyle(bool style)
    {
        m_italicStyle = style;
    }

    bool FontBase::isItalicStyle() const
    {
        return m_italicStyle;
    }

    void FontBase::setUnderlineStyle(bool style)
    {
        m_underlineStyle = style;
    }

    bool FontBase::isUnderlineStyle() const
    {
        return m_underlineStyle;
    }

    void FontBase::setStrikethroughStyle(bool style)
    {
        m_strikeStyle = style;
    }

    bool FontBase::isStrikethroughStyle() const
    {
        return m_strikeStyle;
    }

    void FontBase::setDynamicColoring(bool coloring)
    {
        m_coloring = coloring;
    }

    bool FontBase::isDynamicColoring() const
    {
        return m_coloring;
    }

    SDL_Color FontBase::getColor() const
    {
        return mColor;
    }

    int32_t FontBase::getStringIndexAt(const std::string& text, int32_t x) const
    {
        assert(utf8::is_valid(text.begin(), text.end()));

        std::string::const_iterator cur;
        if (text.empty()) {
            return 0;
        }
        if (x <= 0) {
            return 0;
        }

        cur = text.begin();

        utf8::next(cur, text.end());

        std::string buff;
        while (cur != text.end()) {
            buff = std::string(text.begin(), cur);

            if (getWidth(buff) > x) {
                return buff.size();
            } else {
                utf8::next(cur, text.end());
            }
        }

        if (x > getWidth(text)) {
            return text.size();
        } else {
            return buff.size();
        }
    }

    Image* FontBase::getAsImage(const std::string& text)
    {
        Image* image = m_pool.getRenderedText(this, text);
        if (image == nullptr) {
            SDL_Surface* textSurface = renderString(text);

            image = RenderBackend::instance()->createImage(textSurface);
            m_pool.addRenderedText(this, text, image);
        }
        return image;
    }

    Image* FontBase::getAsImageMultiline(const std::string& text)
    {
        Image* image = m_pool.getRenderedText(this, text);
        if (image != nullptr) {
            return image;
        }

        constexpr uint32_t newline_codepoint = '\n';

        std::vector<SDL_Surface*> lines;
        lines.reserve(16); // TODO: 16 is a heuristic value

        int32_t render_width = 0;

        using Utf8Iter = utf8::iterator<std::string::const_iterator>;
        Utf8Iter it(text.begin(), text.begin(), text.end());
        const Utf8Iter end(text.end(), text.begin(), text.end());

        while (it != end) {
            std::string line;
            line.reserve(64); // TODO: 64 is a heuristic value

            // Build one line
            while (it != end && *it != newline_codepoint) {
                utf8::append(*it, std::back_inserter(line));
                ++it;
            }

            // std::cout << "Line:" << line << '\n';

            // Skip newline if present
            if (it != end && *it == newline_codepoint) {
                ++it;
            }

            SDL_Surface* text_surface = renderString(line);

            // Safety: skip empty lines
            if (text_surface == nullptr) {
                continue;
            }

            render_width = std::max(render_width, text_surface->w);
            lines.emplace_back(text_surface);
        }

        const int32_t row_height    = getRowSpacing() + getHeight();
        const int32_t render_height = row_height * static_cast<int32_t>(lines.size());

        SDL_Surface* final_surface =
            SDL_CreateRGBSurface(0, render_width, render_height, 32, RMASK, GMASK, BMASK, AMASK);

        if (final_surface == nullptr) {
            for (auto* surf : lines) {
                SDL_FreeSurface(surf);
            }
            throw SDLException(std::string("CreateRGBSurface failed: ") + SDL_GetError());
        }

        SDL_FillRect(final_surface, nullptr, 0x00000000);

        int32_t ypos = 0;
        for (auto* line_surf : lines) {
            SDL_Rect dst_rect{0, ypos, line_surf->w, line_surf->h};

            // Disable alpha blending
            // SDL_SetAlpha(*i,0,SDL_ALPHA_OPAQUE);
            SDL_SetSurfaceBlendMode(line_surf, SDL_BLENDMODE_NONE);
            SDL_BlitSurface(line_surf, nullptr, final_surface, &dst_rect);

            ypos += row_height;
            SDL_FreeSurface(line_surf);
        }

        image = RenderBackend::instance()->createImage(final_surface);
        m_pool.addRenderedText(this, text, image);

        return image;
    }

    std::string FontBase::splitTextToWidth(const std::string& text, int32_t render_width)
    {
        const uint32_t whitespace  = ' ';
        const uint8_t newline_utf8 = '\n';
        uint32_t newline           = 0;
        utf8::utf8to32(&newline_utf8, &newline_utf8 + 1, &newline);
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

            bool haveNewLine = false;
            while (getWidth(line) < render_width && pos != text.end()) {
                uint32_t codepoint = utf8::next(pos, text.end());
                if (codepoint == whitespace && !line.empty()) {
                    break_pos.emplace_back(line.length(), pos);
                }

                if (codepoint != newline) {
                    utf8::append(codepoint, back_inserter(line));
                }

                // Special case: Already newlines in string:
                if (codepoint == newline) {
                    output.append(line);
                    line        = "";
                    haveNewLine = true;
                    break;
                }
            }
            if (haveNewLine) {
                continue;
            }

            if (pos == text.end()) {
                break;
            }

            if (break_pos.empty()) {
                // No break position and line length smaller than 2
                // means the renderwidth is really screwed. Just continue
                // appending single character lines.
                if (utf8::distance(line.begin(), line.end()) <= 1 && line != "\n") {
                    output.append(line);
                    continue;
                }

                if (line == "\n") {
                    ++pos;
                }

                // We can't do hyphenation here,
                // so we just retreat one character :-(
                // FIXME
                // line = line.erase(line.length() - 1);
                //--pos;
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
