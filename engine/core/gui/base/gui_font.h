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

#ifndef FIFE_GUI_FONT_H
#define FIFE_GUI_FONT_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/font.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/fonts/abstractfont.h"


namespace FIFE {

	class GuiFont : public gcn::Font, public AbstractFont {
	public:
		/** Constructor
		 *  Takes the ownership of given font
		 */
		GuiFont(AbstractFont* font);
		virtual ~GuiFont();

		int getStringIndexAt(const std::string& text, int x) const;
		void drawString(gcn::Graphics* graphics, const std::string& text, int x, int y);
		void drawMultiLineString(gcn::Graphics* graphics, const std::string& text, int x, int y);
		std::string splitTextToWidth (const std::string& text, int render_width);

		void setRowSpacing (int spacing);
		int getRowSpacing() const;
		void setGlyphSpacing(int spacing);
		int getGlyphSpacing() const;
		void setAntiAlias(bool antiAlias);
		bool isAntiAlias();
		Image* getAsImage(const std::string& text);
		Image* getAsImageMultiline(const std::string& text);
		void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);
		SDL_Color getColor() const;
		int getWidth(const std::string& text) const;
		int getHeight() const;
		void invalidate();

	private:
		AbstractFont* m_font;
	};
}

#endif
