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

// 3rd party library includes
#include <boost/filesystem/convenience.hpp>
#include <boost/scoped_array.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "imagefontbase.h"

namespace FIFE {

	ImageFontBase::ImageFontBase() : FontBase() {
	}

	ImageFontBase::~ImageFontBase() {
		type_glyphs::iterator i = m_glyphs.begin();
		for(; i != m_glyphs.end(); ++i) {
			SDL_FreeSurface(i->second.surface);
		}
		
	}

	int ImageFontBase::getWidth(const std::string& text) const {
		int w = 0;

		for(size_t i=0; i!= text.size(); ++i) {
			type_glyphs::const_iterator it = m_glyphs.find( text[i] );

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

	int ImageFontBase::getHeight() const {
		return mHeight;
	}

	SDL_Surface *ImageFontBase::renderString(const std::string& text) {
		SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			getWidth(text),getHeight(),32,
			0xff000000,0x00ff0000,0x0000ff00,0x000000ff);

		SDL_FillRect(surface,0,0x00000000);

		SDL_Rect dst;
		dst.x = dst.y = 0;
		s_glyph *glyph = 0;

		for(size_t i=0; i!= text.size(); ++i) {
			type_glyphs::iterator it = m_glyphs.find( text[i] );

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

	void ImageFontBase::setColor(Uint8 r, Uint8 g, Uint8 b) {
	}
}
