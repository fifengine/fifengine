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

// 3rd party library includes
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "imagefontbase.h"

namespace FIFE {

	ImageFontBase::ImageFontBase() : FontBase(), m_height(0) {
	}

	ImageFontBase::~ImageFontBase() {
		type_glyphs::iterator i = m_glyphs.begin();
		for(; i != m_glyphs.end(); ++i) {
			SDL_FreeSurface(i->second.surface);
		}
		
	}

	int32_t ImageFontBase::getWidth(const std::string& text) const {
		int32_t w = 0;
		std::string::const_iterator text_it = text.begin();
		while(text_it != text.end()) {
			uint32_t codepoint = utf8::next(text_it,text.end());
			type_glyphs::const_iterator it = m_glyphs.find( codepoint );

			if( it != m_glyphs.end() ) {
				w += it->second.surface->w + getGlyphSpacing();
				continue;
			}

			if( m_placeholder.surface ) {
				w += m_placeholder.surface->w + getGlyphSpacing();
			}
		}
		return w;
	}

	int32_t ImageFontBase::getHeight() const {
		return m_height;
	}

	SDL_Surface *ImageFontBase::renderString(const std::string& text) {
		SDL_Surface *surface = SDL_CreateRGBSurface(0,
			getWidth(text),getHeight(),32,
			RMASK, GMASK, BMASK ,AMASK);

		SDL_FillRect(surface,0,0x00000000);

		SDL_Rect dst;
		dst.x = dst.y = 0;
		s_glyph *glyph = 0;

		std::string::const_iterator text_it = text.begin();
		while(text_it != text.end()) {
			uint32_t codepoint = utf8::next(text_it,text.end());
			type_glyphs::iterator it = m_glyphs.find( codepoint );

			if( it == m_glyphs.end() ) {
				if( !m_placeholder.surface ) {
					continue;
				}
				glyph = &m_placeholder;
			} else {
				glyph = &(it->second);
			}
			dst.y  = glyph->offset.y;
			dst.x += glyph->offset.x;

			SDL_BlitSurface(glyph->surface,0,surface,&dst);
			dst.x += glyph->surface->w + getGlyphSpacing();
		}

		return surface;
	}

	void ImageFontBase::setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a) {
	}
}
