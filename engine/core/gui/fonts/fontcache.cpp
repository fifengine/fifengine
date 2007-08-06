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

// Standard C++ library includes
#include <algorithm>

// Platform specific includes

// 3rd party library includes
#include <boost/bind.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/image.h"
#include "util/time/timemanager.h"
#include "util/log.h"

#include "fontbase.h"
#include "fontcache.h"

namespace FIFE {

	FontCache::FontCache(size_t cacheSize) {
		m_cacheMaxSize = cacheSize;
		m_cacheSize = 0;

		m_collectTimer.setInterval( 1000 * 60 );
		m_collectTimer.setCallback( boost::bind( &FontCache::removeOldEntries, this) );
	}

	FontCache::~FontCache() {
		type_cache::iterator it= m_cache.begin();
		for(;it != m_cache.end(); ++it) {
			delete it->image;
		}
	}

	Image* FontCache::getRenderedText( FontBase* fontbase, const std::string& text) {
		SDL_Color c = fontbase->getColor();

		type_cache::iterator it= m_cache.begin();
		for(;it != m_cache.end(); ++it) {
			if( it->antialias != fontbase->isAntiAlias() )
				continue;

			if( it->glyph_spacing != fontbase->getGlyphSpacing() )
				continue;

			if( it->row_spacing != fontbase->getRowSpacing() )
				continue;

			if( it->color.r != c.r || it->color.g != c.g || it->color.b != c.b  )
				continue;

			if( it->text != text )
				continue;

			// Stay sorted after access time
			it->timestamp = TimeManager::instance()->getTime();
			m_cache.push_front( *it );
			m_cache.erase( it );

			return m_cache.front().image;
		}
		return 0;
	}

	void FontCache::addRenderedText( FontBase* fontbase,const std::string& text, Image* image) {
		// Construct a entry and add it.
		s_cache_entry centry;
		centry.antialias = fontbase->isAntiAlias();
		centry.glyph_spacing = fontbase->getGlyphSpacing();
		centry.row_spacing = fontbase->getRowSpacing();
		centry.text = text;
		centry.color = fontbase->getColor();
		centry.image = image;
		centry.timestamp = TimeManager::instance()->getTime();
		m_cache.push_front( centry );

		// Some minimal amount of entries -> start collection timer
		// Don't have a timer active if only _some_ text is cached.
		if( m_cacheSize >= m_cacheMaxSize/10 )
			m_collectTimer.start();

		// Maintain max cache size
		if( m_cacheSize < m_cacheMaxSize ) {
			m_cacheSize++;
			return;
		} else {
			delete m_cache.back().image;
			m_cache.pop_back();
		}
	}

	void FontCache::removeOldEntries() {
// 		Log("fontcache")
// 			<< "Removing old entries from cache "
// 			<< " now: " << m_cacheSize
// 			<< " max: " << m_cacheMaxSize;

		type_cache::iterator tmp,it = m_cache.begin();
		uint32_t now = TimeManager::instance()->getTime();
		for(;it != m_cache.end(); ++it) {
			if( (now - it->timestamp) > 1000*60 ) {
				tmp = it;
				++tmp;
				delete it->image;
				m_cache.erase(it);
				--m_cacheSize;
				it = tmp;
			}
		}

// 		Log("fontcache")
// 			<< "Removing old entries from cache (DONE) "
// 			<< " now: " << m_cacheSize
// 			<< " max: " << m_cacheMaxSize;

		// Stop if nothing can grow old =)
		if( m_cacheSize == 0 )
			m_collectTimer.stop();
	}
}
