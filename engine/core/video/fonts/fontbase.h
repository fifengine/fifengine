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

#ifndef FIFE_FONTS_FONTBASE_H
#define FIFE_FONTS_FONTBASE_H

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
#include "textrenderpool.h"
#include "abstractfont.h"

struct SDL_Surface;
namespace FIFE {

	/** Abstract Font Base Class
	 *  Uses a pool for rendered strings.
	 *  @see TextRenderPool
	 */
	class FontBase: public AbstractFont {
	public:
		FontBase();
		virtual ~FontBase() {};

		void invalidate();
		void setRowSpacing (int spacing);
		int getRowSpacing() const;
		void setGlyphSpacing(int spacing);
		int getGlyphSpacing() const;
		void setAntiAlias(bool antiAlias);
		bool isAntiAlias();
		virtual int getStringIndexAt(const std::string &text, int x) const;

		Image* getAsImage(const std::string& text);
		Image* getAsImageMultiline(const std::string& text);
		std::string splitTextToWidth (const std::string& text, int render_width);

		SDL_Color getColor() const;

		virtual SDL_Surface* renderString(const std::string& text) = 0;

	protected:
		TextRenderPool m_pool;

		SDL_Color mColor;
		int mGlyphSpacing;
		int mRowSpacing;

		std::string mFilename;
		bool m_antiAlias;
	};

}

#endif
