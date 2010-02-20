/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"
#include "video/imagepool.h"

#include "subimagefont.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	SubImageFont::SubImageFont(const std::string& filename, const std::string& glyphs, ImagePool& pool) 
		: ImageFontBase(), m_pool(pool) {

		FL_LOG(_log, LMsg("guichan_image_font, loading ") << filename << " glyphs " << glyphs);

		int image_id = m_pool.addResourceFromFile(filename);
		Image& img = dynamic_cast<Image&>(m_pool.get(image_id));
		SDL_Surface* surface = img.getSurface();
		m_colorkey = RenderBackend::instance()->getColorKey();

		if( !surface ) {
			throw CannotOpenFile(filename);
		}

		// Make sure we get 32bit RGB
		// and copy the Pixelbuffers surface
		SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE,
			surface->w,surface->h,32,
			RMASK, GMASK, BMASK ,NULLMASK);

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
		uint32_t colorkey = SDL_MapRGB(surface->format, m_colorkey.r, m_colorkey.g, m_colorkey.b);

		// if colorkey feature is not enabled then manually find the color key in the font
		if (!RenderBackend::instance()->isColorKeyEnabled()) {	
			while(x < surface->w && pixels[x] == separator) {
				++x;
			}
			
			colorkey = pixels[x];
		}
		
		// Disable alpha blending, so that we use color keying
		SDL_SetAlpha(surface,0,255);
		SDL_SetColorKey(surface,SDL_SRCCOLORKEY,colorkey);

		FL_DBG(_log, LMsg("image_font")
			<< " glyph separator is " 
			<< pprint(reinterpret_cast<void*>(separator))
			<< " transparent color is " 
			<< pprint(reinterpret_cast<void*>(colorkey)));

		// Finally extract all glyphs
		std::string::const_iterator text_it = glyphs.begin();
		while(text_it != glyphs.end()) {
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
					RMASK, GMASK, BMASK ,NULLMASK);

			SDL_FillRect(tmp,0,colorkey);
			SDL_BlitSurface(surface,&src,tmp,0);

			// Disable alpha blending, so that we use colorkeying
			SDL_SetAlpha(tmp,0,255);
			SDL_SetColorKey(tmp,SDL_SRCCOLORKEY,colorkey);


			uint32_t codepoint = utf8::next(text_it, glyphs.end());
			m_glyphs[ codepoint ].surface = tmp;

			x += w;
		}

		// Set placeholder glyph
		// This should actually work ith utf8.
		if( m_glyphs.find('?') != m_glyphs.end() ) {
			m_placeholder = m_glyphs['?'];
		} else {
			m_placeholder.surface = 0;
		}

		mHeight = surface->h;
		SDL_FreeSurface(surface);
	}


}

