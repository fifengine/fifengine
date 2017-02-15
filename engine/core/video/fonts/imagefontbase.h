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

#ifndef FIFE_FONTS_IMAGEFONTBASE_H
#define FIFE_FONTS_IMAGEFONTBASE_H

// Standard C++ library includes
#include <map>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/point.h"

#include "fontbase.h"

namespace FIFE {

	/** ImageFont base class
	 *
	 *  Just set the glyphs/placeholder in any derived class and the rendering
	 *  is handled by this class. Also frees all glyph surfaces on destruction.
	 */
	class ImageFontBase: public FontBase {
		public:

			/**
			 * Constructor.
			 */
			ImageFontBase();

			/**
			 * Destructor.
			 */
			virtual ~ImageFontBase();

			/** Get the width in pixels a given text would occupy
			 *  @param text The text that should be measured.
			 */
			virtual int32_t getWidth(const std::string& text) const;

			/** Get the height in pixels a text line would occupy
			 */
			virtual int32_t getHeight() const;

			virtual SDL_Surface *renderString(const std::string& text);
			virtual void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);

		protected:
			// A glyph (visible character) 
			typedef struct {
				// The offset of the glyph relative to the top-left corner.
				Point offset;
				// The glyphs image 
				// should be with SDL_SRCALPHA off, so that it's just copied over.
				SDL_Surface* surface;
			} s_glyph;

			typedef std::map<int32_t,s_glyph> type_glyphs;
			type_glyphs m_glyphs;

			// The glyph used, when the real glyph is not found
			// Should default to '?'
			s_glyph m_placeholder;

			int32_t m_height;
	};
}

#endif // end GCN_SDLTRUETYPEFONT_HPP
