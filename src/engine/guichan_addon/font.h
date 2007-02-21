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

#ifndef FIFE_GUICHAN_ADDON_FONTBASE_H
#define FIFE_GUICHAN_ADDON_FONTBASE_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <guichan.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fontcache.h"

struct SDL_Surface;
namespace FIFE {


	/** Abstract Font Base Class
	 *  Uses a cache for rendered strings.
	 *  @see FontCache
	 */
	class FontBase : public gcn::Font {
		public:
			FontBase();

			/**
			 * Sets the spacing between rows in pixels. Default is 0 pixels.
			 * The spacing can be negative.
			 *
			 * @param spacing the spacing in pixels.
			 */
			virtual void setRowSpacing (int spacing);

			/**
			 * Gets the spacing between rows in pixels.
			 *
			 * @return the spacing.
			 */
			virtual int getRowSpacing() const;

			/**
			 * Sets the spacing between letters in pixels. Default is 0 pixels.
			 * The spacing can be negative.
			 *
			 * @param spacing the spacing in pixels.
			 */
			virtual void setGlyphSpacing(int spacing);

			/**
			 * Gets the spacing between letters in pixels.
			 *
			 * @return the spacing.
			 */
			virtual int getGlyphSpacing() const;

			/**
			 * Sets the use of anti aliasing..
			 *
			 * @param antaAlias true for use of antia aliasing.
			 */
			virtual void setAntiAlias(bool antiAlias);

			/**
			 * Checks if anti aliasing is used.
			 *
			 * @return true if anti aliasing is used.
			 */
			virtual bool isAntiAlias();

			virtual void drawString(gcn::Graphics* graphics, const std::string& text, int x, int y);
			virtual int getStringIndexAt(const std::string &text, int x);

			/** Render a string to a SDL surface
			 *  Overwrite this method, in your font implementation.
			 *  The rsulting SDL surface is cached, so it's not that time critical.
			 */
			virtual SDL_Surface* renderString(const std::string& text) = 0;


			virtual void setColor(uint8_t r,uint8_t g,uint8_t b) = 0;
			SDL_Color getColor() const;

			/** Static factory method
			 * @bug This is ugly.
			 */
			static FontBase* load(const std::string& filename,int size=12, const std::string& glyphs ="");

		protected:
			FontCache m_cache;

			SDL_Color mColor;
			int mGlyphSpacing;
			int mRowSpacing;

			std::string mFilename;
			bool m_antiAlias;
	};

}

#endif
