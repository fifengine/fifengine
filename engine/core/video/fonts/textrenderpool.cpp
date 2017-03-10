/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
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

#include "fontbase.h"
#include "textrenderpool.h"

namespace FIFE {

	TextRenderPool::TextRenderPool(size_t poolSize) {
		m_poolMaxSize = poolSize;
		m_poolSize = 0;

		m_collectTimer.setInterval( 1000 * 60 );
		m_collectTimer.setCallback( boost::bind( &TextRenderPool::removeOldEntries, this) );
	}

	TextRenderPool::~TextRenderPool() {
		type_pool::iterator it= m_pool.begin();
		for(;it != m_pool.end(); ++it) {
			delete it->image;
		}
	}

	Image* TextRenderPool::getRenderedText( FontBase* fontbase, const std::string& text) {
		SDL_Color c = fontbase->getColor();

		type_pool::iterator it= m_pool.begin();
		for(;it != m_pool.end(); ++it) {
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
			m_pool.push_front( *it );
			m_pool.erase( it );

			return m_pool.front().image;
		}
		return 0;
	}

	void TextRenderPool::addRenderedText( FontBase* fontbase,const std::string& text, Image* image) {
		// Construct a entry and add it.
		s_pool_entry centry;
		centry.antialias = fontbase->isAntiAlias();
		centry.glyph_spacing = fontbase->getGlyphSpacing();
		centry.row_spacing = fontbase->getRowSpacing();
		centry.text = text;
		centry.color = fontbase->getColor();
		centry.image = image;
		centry.timestamp = TimeManager::instance()->getTime();
		m_pool.push_front( centry );

		// Some minimal amount of entries -> start collection timer
		// Don't have a timer active if only _some_ text is pooled.
		if( m_poolSize >= m_poolMaxSize/10 )
			m_collectTimer.start();

		// Maintain max pool size
		if( m_poolSize < m_poolMaxSize ) {
			m_poolSize++;
			return;
		} else {
			delete m_pool.back().image;
			m_pool.pop_back();
		}
	}

	void TextRenderPool::removeOldEntries() {

		type_pool::iterator it = m_pool.begin();
		uint32_t now = TimeManager::instance()->getTime();
		while (it != m_pool.end()) {
			if( (now - it->timestamp) > 1000*60 ) {
				delete it->image;
				it = m_pool.erase(it);
				--m_poolSize;
			}
			else {
				++it;
			}
		}

		// Stop if nothing can grow old =)
		if( m_poolSize == 0 )
			m_collectTimer.stop();
	}

	void TextRenderPool::invalidateCachedText() {
		type_pool::iterator it = m_pool.begin();
		while (it != m_pool.end()) {
			it->image->invalidate();
			++it;
		}
	}
}
