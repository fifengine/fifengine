/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_GUICHAN_ADDON_FONTCACHE_H
#define FIFE_GUICHAN_ADDON_FONTCACHE_H

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
namespace FIFE {
	class FontBase;
	class Image;

	/** Generic cache for rendered text
	 *  Caches a number of Images with text, as rendered by a Font.
	 *  Makes sure no more than a maximum number of strings is cached at a time.
	 *  Automatically removes cached strings not used for a minute.
	 *  Doesn't use resources (apart from a minimum) if not used after a while.
	 *
	 *  @todo Should probably use a @c std::map instead of a @c std::list
	 */
	class FontCache {
		public:
			/** Constructor
			 *  Constructs a cache with a maximum of cacheSize entries
			 */
			FontCache(size_t cacheSize = 200);

			/** Destructor
			 */
			~FontCache();

			/** Get a cache string image
			 */
			Image* getRenderedText( FontBase* fontbase, const std::string& text);

			/** Add a cache string image
			 */
			void addRenderedText( FontBase* fontbase, const std::string& text, Image* image);

			/** Remove entries not used since a minute
			 *  Is a timer callback.
			 */
			void removeOldEntries();

		protected:
			typedef struct {
				std::string text;
				SDL_Color color;
				bool antialias;
				int glyph_spacing;
				int row_spacing;
				uint32_t timestamp;

				Image* image;
			} s_cache_entry;

			typedef std::list<s_cache_entry> type_cache;
			type_cache m_cache;
			size_t m_cacheSize;
			size_t m_cacheMaxSize;

			Timer m_collectTimer;
	};
}
#endif
