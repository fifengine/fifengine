// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_TEXTRENDERPOOL_H
#define FIFE_FONTS_TEXTRENDERPOOL_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/time/timer.h"

struct SDL_Surface;
namespace FIFE
{
    class IFont;
    class Image;

    /** Generic pool for rendered text
     *  Caches a number of Images with text, as rendered by a Font.
     *  Makes sure no more than a maximum number of strings is pooled at a time.
     *  Automatically removes pooled strings not used for a minute.
     *  Doesn't use resources (apart from a minimum) if not used after a while.
     */
    class FIFE_API TextRenderPool
    {
        public:
            /** Constructor
             *  Constructs a pool with a maximum of poolSize entries
             */
            explicit TextRenderPool(size_t poolSize = 200);

            TextRenderPool(TextRenderPool const &)            = delete;
            TextRenderPool& operator=(TextRenderPool const &) = delete;

            /** Destructor
             */
            ~TextRenderPool();

            /** Invalidates all cached text images
             */
            void invalidateCachedText();

            /** Get a string image
             */
            Image* getRenderedText(IFont const * font, std::string const & text);

            /** Add a string image
             */
            void addRenderedText(IFont const * font, std::string const & text, Image* image);

            /** Remove entries not used since a minute
             *  Is a timer callback.
             */
            void removeOldEntries();

        protected:
            struct FIFE_API s_pool_entry
            {
                    std::string text;
                    SDL_Color color;
                    bool antialias;
                    int glyph_spacing;
                    int row_spacing;
                    uint64_t timestamp;

                    std::unique_ptr<Image> image;
            };

            /** Composite key identifying a pooled entry: text plus the font
             *  attributes the entry was rendered with. Source of truth for
             *  entry identity.
             */
            struct FIFE_API s_pool_key
            {
                    std::string text;
                    SDL_Color color;
                    bool antialias;
                    int glyph_spacing;
                    int row_spacing;

                    bool operator<(s_pool_key const & other) const
                    {
                        auto const lhs =
                            std::tie(text, color.r, color.g, color.b, color.a, antialias, glyph_spacing, row_spacing);
                        auto const rhs = std::tie(
                            other.text,
                            other.color.r,
                            other.color.g,
                            other.color.b,
                            other.color.a,
                            other.antialias,
                            other.glyph_spacing,
                            other.row_spacing);
                        return lhs < rhs;
                    }
            };

            using type_pool       = std::list<s_pool_entry>;
            using type_pool_index = std::map<s_pool_key, type_pool::iterator>;

            // The list keeps the LRU order (front = most recently used); the index
            // maps each entry key to its list iterator for O(log n) lookup.
            type_pool m_pool;
            type_pool_index m_poolIndex;
            size_t m_poolMaxSize;

            Timer m_collectTimer;

        private:
            static s_pool_key makeKey(IFont const * font, std::string const & text);
            static s_pool_key makeKey(s_pool_entry const & entry);
    };
} // namespace FIFE
#endif
