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

#ifndef FIFE_GUI_FONT_H
#define FIFE_GUI_FONT_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/font.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/fonts/ifont.h"


namespace FIFE {

	class GuiFont : public fcn::Font, public IFont {
	public:
		/** Constructor
		 *  Takes the ownership of given font
		 */
		GuiFont(IFont* font);
		virtual ~GuiFont();

		int32_t getStringIndexAt(const std::string& text, int32_t x) const;
		void drawString(fcn::Graphics* graphics, const std::string& text, int32_t x, int32_t y);
		void drawMultiLineString(fcn::Graphics* graphics, const std::string& text, int32_t x, int32_t y);
		std::string splitTextToWidth (const std::string& text, int32_t render_width);

		void setRowSpacing (int32_t spacing);
		int32_t getRowSpacing() const;
		void setGlyphSpacing(int32_t spacing);
		int32_t getGlyphSpacing() const;
		void setAntiAlias(bool antiAlias);
		bool isAntiAlias() const;
		void setBoldStyle(bool style);
		bool isBoldStyle() const;
		void setItalicStyle(bool style);
		bool isItalicStyle() const;
		void setUnderlineStyle(bool style);
		bool isUnderlineStyle() const;
		void setStrikethroughStyle(bool style);
		bool isStrikethroughStyle() const;
		void setDynamicColoring(bool coloring);
		bool isDynamicColoring() const;
		Image* getAsImage(const std::string& text);
		Image* getAsImageMultiline(const std::string& text);
		void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);
		SDL_Color getColor() const;
		int32_t getWidth(const std::string& text) const;
		int32_t getHeight() const;
		void invalidate();

	private:
		IFont* m_font;
	};
}

#endif
