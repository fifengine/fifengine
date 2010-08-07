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
#include <cassert>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "truetypefont.h"

namespace FIFE {

	TrueTypeFont::TrueTypeFont(const std::string& filename, int size)
		: FIFE::FontBase() {
		mFilename = filename;
		mFont = NULL;

		mFont = TTF_OpenFont(filename.c_str(), size);

		if (mFont == NULL) {
			throw FIFE::CannotOpenFile(filename + " (" + TTF_GetError() + ")");
		}
		mColor.r = mColor.g = mColor.b = mColor.unused = 255;
	}

	TrueTypeFont::~TrueTypeFont() {
		TTF_CloseFont(mFont);
	}

	int TrueTypeFont::getWidth(const std::string& text) const {
		int w, h;
		assert( utf8::is_valid(text.begin(), text.end()) );
		TTF_SizeUTF8(mFont, text.c_str(), &w, &h);
		return w;
	}

	int TrueTypeFont::getHeight() const {
		return TTF_FontHeight(mFont) + getRowSpacing();
	}

	SDL_Surface* TrueTypeFont::renderString(const std::string& text) {
		if( text.empty() ) {
			SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				1,getHeight(),32,
				RMASK, GMASK, BMASK ,AMASK);
			SDL_FillRect(surface,0,0x00000000);
			return surface;
		}

		SDL_Surface* renderedText = 0;
		if (m_antiAlias) {
			renderedText = TTF_RenderUTF8_Blended(mFont, text.c_str(), mColor);
		} else {
			renderedText = TTF_RenderUTF8_Solid(mFont, text.c_str(), mColor);
		}
		// Workaround for a freetype bug, see here:
		// http://www.nabble.com/SDL_ttf-and-DPMSDisable-bug-is-back-or-still-there-to9578884.html
		if (renderedText == 0 && !m_antiAlias) {
			renderedText = TTF_RenderUTF8_Blended(mFont, text.c_str(), mColor);
		}
		// Still could not render? Something went horribly wrong!
		if (renderedText == 0) {
			throw FIFE::SDLException(TTF_GetError());
		}
		return renderedText;
	}

	void TrueTypeFont::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
		mColor.r = r;
		mColor.g = g;
		mColor.b = b;
		mColor.unused = a;
	}
}
