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
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"
#include "util/log.h"
#include "util/rect.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"

#include "aafont.h"

namespace FIFE {

	AAImageFont::AAImageFont(const std::string& filename) 
		: SDLImageFont() {

		Log("aa_image_font")
			<< " loading " << filename;

		RawDataPtr data =  VFS::instance()->open(filename);

		std::string signature(data->readString(4));

		Log("image_font")
			<< "font '" << filename << "' signature is " << signature;

		if( signature != "AAFF"  ) {
			throw InvalidFormat(filename + " has no AAFF signature");
		}

		// read global info
		mHeight = data->read16Big();
		uint16_t gap_h   = data->read16Big();
		uint16_t space_w = data->read16Big();
		uint16_t gap_v   = data->read16Big();

		setGlyphSpacing( gap_h );
		setRowSpacing( gap_v );

		std::vector<uint16_t> glyph_w,glyph_h;
		std::vector<uint32_t> glyph_offset;

		// Read the glyph descriptions
		for(size_t i = 0; i < 256; ++i) {
			glyph_w.push_back( data->read16Big() );
			glyph_h.push_back( data->read16Big() );
			glyph_offset.push_back( data->read32Big() + 0x080C );
		}

		SDL_Surface *glyph;
		for(size_t i = 0; i < 256; ++i) {
// 			Log("image_font")
// 				<< "AAF glyph " << char(i)
// 				<< " at " << glyph_offset[i]
// 				<< " w,h " <<  glyph_w[i] << ", " << glyph_h[i];

			uint16_t size = glyph_w[i] * glyph_h[i];

			// Ignore empty glyphs
			if ( size == 0 )
				continue;

			data->setIndex( glyph_offset[i] );

			// Load pixels as 8bit palette style surfaces
			int w = glyph_w[i];
			int h = glyph_h[i];
			glyph = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,8,0,0,0,0);
			uint8_t *pixels = static_cast<uint8_t*>(glyph->pixels);
			for(int j = 0; j != h; ++j) {
				data->readInto( pixels, w );
				pixels += glyph->pitch;
			}

			SDL_SetAlpha(glyph,0,255);
			SDL_SetColorKey(glyph,SDL_SRCCOLORKEY,0x00000000);

			m_glyphs[ i ].surface = glyph;
			m_glyphs[ i ].offset.y = getHeight() - glyph_h[i];
			m_glyphs[ i ].offset.x = 0;
		}

		// Set space glyph.
		if( m_glyphs.find(' ') == m_glyphs.end() ) {
			glyph = SDL_CreateRGBSurface(SDL_SWSURFACE,space_w,1,8,0,0,0,0);
			SDL_SetAlpha(glyph,0,255);
			SDL_SetColorKey(glyph,SDL_SRCCOLORKEY,0x00000000);
			m_glyphs[ ' ' ].surface = glyph;
		}

		// Set placeholder glyph
		if( m_glyphs.find('?') != m_glyphs.end() ) {
			m_placeholder = m_glyphs['?'];
		} else {
			m_placeholder.surface = 0;
		}

		// Initialize palettes with this
		setColor( 255, 255, 255 );
	}

	void AAImageFont::setColor(Uint8 r, Uint8 g, Uint8 b) {
		// This is only of interest for AA Fonts
		mColor.r = r;
		mColor.g = g;
		mColor.b = b;

		// AAF uses palette indices 0..9 with a growing brightness
		for(size_t i=0; i != 10; ++i) {
			m_aaf_palette[i].r = (r/10) * i;
			m_aaf_palette[i].g = (g/10) * i;
			m_aaf_palette[i].b = (b/10) * i;
		}

		// Reset palette info
		type_glyphs::iterator it = m_glyphs.begin();
		if( it == m_glyphs.end() || it->second.surface->format->palette == 0 )
			return;
		for(; it != m_glyphs.end(); ++it) {
			SDL_SetPalette( it->second.surface, SDL_LOGPAL|SDL_PHYSPAL, m_aaf_palette, 0,9 );
		}
		if( m_placeholder.surface ) {
			SDL_SetPalette( m_placeholder.surface, SDL_LOGPAL|SDL_PHYSPAL, m_aaf_palette, 0,9 );
		}
	}


}

