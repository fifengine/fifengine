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

#ifndef FIFE_FONTS_TRUETYPEFONT_H
#define FIFE_FONTS_TRUETYPEFONT_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes
#include <SDL_ttf.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "fontbase.h"

namespace FIFE {

	/**
	 * SDL True Type Font implementation of Font. It uses the SDL_ttf library
	 * to display True Type Fonts with SDL.
	 *
	 * NOTE: You must initialize the SDL_ttf library before using this
	 *       class. Also, remember to call the SDL_ttf libraries quit
	 *       function.
	 *
	 * Original author of this class is Walluce Pinkham. Some modifications
	 * made by the Guichan team, and additonal modifications by the FIFE team.
	 */
	class TrueTypeFont: public FontBase {
		public:

			/**
			 * Constructor.
			 *
			 * @param filename the filename of the True Type Font.
			 * @param size the size the font should be in.
			 */
			TrueTypeFont(const std::string& filename, int32_t size);

			/**
			 * Destructor.
			 */
			virtual ~TrueTypeFont();

			// Inherited from Font

			virtual int32_t getWidth(const std::string& text) const;

			virtual int32_t getHeight() const;

			virtual void setBoldStyle(bool style);
			virtual void setItalicStyle(bool style);
			virtual void setUnderlineStyle(bool style);
			virtual void setStrikethroughStyle(bool style);
			virtual int32_t getFontStyleMask() const;

			virtual SDL_Surface *renderString(const std::string& text);

			virtual void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);

		protected:
			TTF_Font* mFont;

			int32_t mFontStyle;
	};
}

#endif // 
