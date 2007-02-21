/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

// Platform specific includes

// 3rd party library includes
#include <boost/filesystem/convenience.hpp>
#include <guichan.hpp>

// FIFE includes
#include "font.h"

#include "video/rect.h"
#include "video/rendermanager.h"
#include "video/renderbackend.h"
#include "video/screen.h"
#include "video/image.h"

#include "sdltruetypefont.hpp"
#include "aafont.h"
#include "guichanfont.h"

namespace FIFE {

	FontBase::FontBase() : gcn::Font(), m_cache() {
		mRowSpacing = 0;
		mGlyphSpacing = 0;
		m_antiAlias = true;
	}

	void FontBase::setRowSpacing(int spacing) {
		mRowSpacing = spacing;
	}

	int FontBase::getRowSpacing() const {
		return mRowSpacing;
	}

	void FontBase::setGlyphSpacing(int spacing) {
		mGlyphSpacing = spacing;
	}

	int FontBase::getGlyphSpacing() const {
		return mGlyphSpacing;
	}

	void FontBase::setAntiAlias(bool antiAlias) {
		m_antiAlias = antiAlias;
	}

	bool FontBase::isAntiAlias() {
		return m_antiAlias;
	}

	SDL_Color FontBase::getColor() const {
		return mColor;
	}

	void FontBase::drawString(gcn::Graphics* graphics, const std::string& text, const int x, const int y) {
		if (text == "") {
			return;
		}

		int yoffset = getRowSpacing() / 2;

		const gcn::ClipRectangle& clip = CRenderBackend()->getMainScreen()->getCurrentClipArea();
		FIFE::Rect rect;
		rect.x = x + clip.xOffset;
		rect.y = y + clip.yOffset + yoffset;
		rect.w = getWidth(text);
		rect.h = getHeight();

		if (!rect.intersects(Rect(clip.x,clip.y,clip.width,clip.height)) ) {
			return;
		}

		FIFE::Image* image = m_cache.getRenderedText( this, text );
		if (image == 0) {
			SDL_Surface* textSurface = renderString(text);
			image = CRenderBackend()->createStaticImageFromSDL(textSurface);
			m_cache.addRenderedText( this, text, image );
		}

		image->render(rect, CRenderBackend()->getMainScreen());
	}

	int FontBase::getStringIndexAt(const std::string &text, int x) {
		for (int i = 0; i < static_cast<int>(text.size()); ++i) {
			if (getWidth(text.substr(0,i)) > x) {
				return i-1;
			}
		}
		return text.length();
	}

	FontBase* FontBase::load(const std::string& filename, int size, const std::string& glyphs) {
		if( boost::filesystem::extension(filename) == ".ttf" ) {
			return new gcn::SDLTrueTypeFont(filename,size);
		}
		try {
			return new GuichanImageFont(filename,glyphs);
		} catch( CannotOpenFile&  ) {
			return new AAImageFont(filename);
		}
	}

}


