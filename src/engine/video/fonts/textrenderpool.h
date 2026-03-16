// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FONTS_TEXTRENDERPOOL_H
#define FIFE_FONTS_TEXTRENDERPOOL_H

// Standard C++ library includes
#include <list>
#include <string>

// Platform specific includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/time/timer.h"

struct SDL_Surface;
namespace FIFE
{
    class FontBase;
    class Image;

    /** Generic pool for rendered text
     *  Caches a number of Images with text, as rendered by a Font.
     *  Makes sure no more than a maximum number of strings is pooled at a time.
     *  Automatically removes pooled strings not used for a minute.
     *  Doesn't use resources (apart from a minimum) if not used after a while.
     *
     *  @todo Should probably use a @c std::map instead of a @c std::list
     */
    class TextRenderPool
    {
    public:
        /** Constructor
         *  Constructs a pool with a maximum of poolSize entries
         */
        explicit TextRenderPool(size_t poolSize = 200);

        /** Destructor
         */
        ~TextRenderPool();

        /** Invalidates all cached text images
         */
        void invalidateCachedText();

        /** Get a string image
         */
        Image* getRenderedText(const FontBase* fontbase, const std::string& text);

        /** Add a string image
         */
        void addRenderedText(const FontBase* fontbase, const std::string& text, Image* image);

        /** Remove entries not used since a minute
         *  Is a timer callback.
         */
        void removeOldEntries();

    protected:
        typedef struct
        {
            std::string text;
            SDL_Color color;
            bool antialias;
            int glyph_spacing;
            int row_spacing;
            uint32_t timestamp;

            Image* image;
        } s_pool_entry;

        typedef std::list<s_pool_entry> type_pool;
        type_pool m_pool;
        size_t m_poolSize;
        size_t m_poolMaxSize;

        Timer m_collectTimer;
    };
} // namespace FIFE
#endif
