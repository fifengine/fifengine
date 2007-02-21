/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/*      _______   __   __   __   ______   __   __   _______   __   __
 *     / _____/\ / /\ / /\ / /\ / ____/\ / /\ / /\ / ___  /\ /  |\/ /\
 *    / /\____\// / // / // / // /\___\// /_// / // /\_/ / // , |/ / /
 *   / / /__   / / // / // / // / /    / ___  / // ___  / // /| ' / /
 *  / /_// /\ / /_// / // / // /_/_   / / // / // /\_/ / // / |  / /
 * /______/ //______/ //_/ //_____/\ /_/ //_/ //_/ //_/ //_/ /|_/ /
 * \______\/ \______\/ \_\/ \_____\/ \_\/ \_\/ \_\/ \_\/ \_\/ \_\/
 *
 * Copyright (c) 2004, 2005 darkbits                        Js_./
 * Per Larsson a.k.a finalman                          _RqZ{a<^_aa
 * Olof Naess� a.k.a jansem/yakslem                _asww7!uY`>  )\a//
 *                                                 _Qhm`] _f "'c  1!5m
 * Visit: http://guichan.darkbits.org             )Qk<P ` _: :+' .'  "{[
 *                                               .)j(] .d_/ '-(  P .   S
 * License: (BSD)                                <Td/Z <fP"5(\"??"\a.  .L
 * Redistribution and use in source and          _dV>ws?a-?'      ._/L  #'
 * binary forms, with or without                 )4d[#7r, .   '     )d`)[
 * modification, are permitted provided         _Q-5'5W..j/?'   -?!\)cam'
 * that the following conditions are met:       j<<WP+k/);.        _W=j f
 * 1. Redistributions of source code must       .$%w\/]Q  . ."'  .  mj$
 *    retain the above copyright notice,        ]E.pYY(Q]>.   a     J@\
 *    this list of conditions and the           j(]1u<sE"L,. .   ./^ ]{a
 *    following disclaimer.                     4'_uomm\.  )L);-4     (3=
 * 2. Redistributions in binary form must        )_]X{Z('a_"a7'<a"a,  ]"[
 *    reproduce the above copyright notice,       #}<]m7`Za??4,P-"'7. ).m
 *    this list of conditions and the            ]d2e)Q(<Q(  ?94   b-  LQ/
 *    following disclaimer in the                <B!</]C)d_, '(<' .f. =C+m
 *    documentation and/or other materials      .Z!=J ]e []('-4f _ ) -.)m]'
 *    provided with the distribution.          .w[5]' _[ /.)_-"+?   _/ <W"
 * 3. Neither the name of Guichan nor the      :$we` _! + _/ .        j?
 *    names of its contributors may be used     =3)= _f  (_yQmWW$#(    "
 *    to endorse or promote products derived     -   W,  sQQQQmZQ#Wwa]..
 *    from this software without specific        (js, \[QQW$QWW#?!V"".
 *    prior written permission.                    ]y:.<\..          .
 *                                                 -]n w/ '         [.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT       )/ )/           !
 * HOLDERS AND CONTRIBUTORS "AS IS" AND ANY         <  (; sac    ,    '
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING,               ]^ .-  %
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF            c <   r
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR            aga<  <La
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          5%  )P'-3L
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR        _bQf` y`..)a
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          ,J?4P'.P"_(\?d'.,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES               _Pa,)!f/<[]/  ?"
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT      _2-..:. .r+_,.. .
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     ?a.<%"'  " -'.a_ _,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION)                     ^
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <boost/filesystem/convenience.hpp>
#include <boost/scoped_array.hpp>
#include <SDL.h>
#include <SDL_image.h>

// FIFE includes
#include "exception.h"
#include "debugutils.h"
#include "log.h"
#include "video/image.h"
#include "video/pixelbuffer.h"
#include "video/rect.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "imagecache.h"

#include "sdlimagefont.hpp"

namespace gcn {

	SDLImageFont::SDLImageFont(const std::string& filename, const std::string& glyphs) 
		: FontBase() {
		FIFE::Log("image_font")
			<< " loading " << filename << " glyphs " << glyphs;

		// Get DATA from the VFS.
		FIFE::VFS* vfs = FIFE::VFS::instance();
		FIFE::RawDataPtr data = vfs->open(filename);

		std::string signature(data->readString(4));
		data->setIndex(0);

		FIFE::Log("image_font")
			<< "Font '" << filename << "' signature is " << signature;

		try {
			if( signature == "AAFF"  ) {
				loadAAFont(data);
			} else {
				loadGuichanFont(filename,glyphs);
			}

		} catch( const FIFE::CannotOpenFile& ) {
			// Re-throw, but add filename
			throw FIFE::CannotOpenFile(filename);
		}

	}

	SDLImageFont::~SDLImageFont() {
		type_glyphs::iterator i = m_glyphs.begin();
		for(; i != m_glyphs.end(); ++i) {
			SDL_FreeSurface(i->second.surface);
		}
		
	}

	void SDLImageFont::loadGuichanFont(const std::string& filename, const std::string& glyphs) {

		// FIXME: This Loading procedure is a bit long winded ... 
		size_t image_id = FIFE::ImageCache::instance()->addImageFromFile(filename);
		FIFE::Image* image = dynamic_cast<FIFE::Image*>(FIFE::ImageCache::instance()->getImage(image_id));

		if( image == 0 ) {
			throw FIFE::CannotOpenFile(filename);
		}

		if( !image->getPixelBuffer() ) {
			FIFE::ImageCache::instance()->unloadImage( image_id );
			image = dynamic_cast<FIFE::Image*>(FIFE::ImageCache::instance()->getImage(image_id));
		}

		if( image == 0 || !image->getPixelBuffer() ) {
			throw FIFE::CannotOpenFile(filename);
		}

		SDL_Surface* surface = image->getPixelBuffer()->getSurface();
		// End of getting the surface ... phew.


		// Make sure we get 32bit RGBA
		// and copy the Pixelbuffers surface
		SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE,
			surface->w,surface->h,32,
			0xff000000,0x00ff0000,0x0000ff00,0x000000ff);

		SDL_BlitSurface(surface,0,tmp,0);
		surface = tmp;

		// Prepare the data for extracting the glyphs.
		uint32_t *pixels = reinterpret_cast<uint32_t*>(surface->pixels);

		int x, w;
		x = 0; w=0;

		SDL_Rect src;

		src.h = surface->h;
		src.y = 0;

		uint32_t separator = pixels[0];
		while(x < surface->w && pixels[x] == separator)
			++x;
		uint32_t colorkey = pixels[x];

		FIFE::Debug("image_font")
			<< " glyph separator is " 
			<< FIFE::pprint(reinterpret_cast<void*>(separator))
			<< " transparent color is " 
			<< FIFE::pprint(reinterpret_cast<void*>(colorkey));

		// Disable alpha blending, so that we use colorkeying
		SDL_SetAlpha(surface,0,255);
		SDL_SetColorKey(surface,SDL_SRCCOLORKEY,colorkey);

		// Finally extract all glyphs
		for(size_t i=0; i != glyphs.size(); ++i) {
			w=0;
			while(x < surface->w && pixels[x] == separator)
				++x;
			if( x == surface->w )
				break;

			while(x + w < surface->w && pixels[x + w] != separator)
				++w;

			src.x = x;
			src.w = w;

			tmp = SDL_CreateRGBSurface(SDL_SWSURFACE,
					w,surface->h,32,
					0xff000000,0x00ff0000,0x0000ff00,0x000000ff);

			SDL_FillRect(tmp,0,0x00000000);
			SDL_BlitSurface(surface,&src,tmp,0);

			// Disable alpha blending, so that we use colorkeying
			SDL_SetAlpha(tmp,0,255);
			SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,colorkey);


			m_glyphs[ glyphs[i] ].surface = tmp;

			x += w;
		}
		SDL_FreeSurface(surface);

		// Set placeholder glyph
		if( m_glyphs.find('?') != m_glyphs.end() ) {
			m_placeholder = m_glyphs['?'];
		} else {
			m_placeholder.surface = 0;
			m_placeholder.image = 0;
		}

		mHeight = surface->h;


	}

	void SDLImageFont::loadAAFont(FIFE::RawDataPtr data) {
		// Skip magic string 'AAFF'
		data->setIndex( 4 );

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
// 			FIFE::Log("image_font")
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
			m_placeholder.image = 0;
		}

		// Initialize palettes with this
		setColor( 255, 255, 255 );
// 		throw FIFE::NotSupported("AA Font not supported yet.");
	}

	int SDLImageFont::getWidth(const std::string& text) const {
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

	int SDLImageFont::getHeight() const {
		return mHeight;
	}

	SDL_Surface *SDLImageFont::renderString(const std::string& text) {
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

	void SDLImageFont::setColor(Uint8 r, Uint8 g, Uint8 b) {
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
			SDL_SetPalette( it->second.surface , SDL_LOGPAL|SDL_PHYSPAL, m_aaf_palette, 0,9 );
		}
		if( m_placeholder.surface ) {
			SDL_SetPalette( m_placeholder.surface , SDL_LOGPAL|SDL_PHYSPAL, m_aaf_palette, 0,9 );
		}
	}
}
