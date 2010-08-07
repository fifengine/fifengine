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
			TrueTypeFont(const std::string& filename, int size);

			/**
			 * Destructor.
			 */
			virtual ~TrueTypeFont();

			// Inherited from Font

			virtual int getWidth(const std::string& text) const;

			virtual int getHeight() const;


			virtual SDL_Surface *renderString(const std::string& text);

			virtual void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);

		protected:
			TTF_Font *mFont;

			int mHeight;
			int mGlyphSpacing;
			int mRowSpacing;

			std::string mFilename;
			bool mAntiAlias;
	};
}

#endif // 
