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

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/rect.h"
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
		mColor.r = mColor.g = mColor.b = 255;
	}

	TrueTypeFont::~TrueTypeFont() {
		TTF_CloseFont(mFont);
	}

	int TrueTypeFont::getWidth(const std::string& text) const {
		int w, h;
		TTF_SizeText(mFont, text.c_str(), &w, &h);

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
			renderedText = TTF_RenderText_Blended(mFont, text.c_str(), mColor);
		} else {
			renderedText = TTF_RenderText_Solid(mFont, text.c_str(), mColor);
		}
		// Workaround for a freetype bug, see here:
		// http://www.nabble.com/SDL_ttf-and-DPMSDisable-bug-is-back-or-still-there-to9578884.html
		if (renderedText == 0 && !m_antiAlias) {
			renderedText = TTF_RenderText_Blended(mFont, text.c_str(), mColor);
		}
		// Still could not render? Something went horribly wrong!
		if (renderedText == 0) {
			throw FIFE::SDLException(TTF_GetError());
		}
		return renderedText;
	}

	void TrueTypeFont::setColor(Uint8 r, Uint8 g, Uint8 b) {
		mColor.r = r;
		mColor.g = g;
		mColor.b = b;
	}
}
