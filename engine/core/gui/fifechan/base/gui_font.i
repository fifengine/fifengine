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

%module fife
%{
#include <fifechan/font.hpp>
#include "gui/fifechan/base/gui_font.h"
#include "video/fonts/ifont.h"
%}

%include "video/fonts/fonts.i"

namespace fcn {
	class Graphics;
	class Font
	{
	public:
		virtual ~Font(){ }
		virtual int32_t getWidth(const std::string& text) const = 0;
		virtual int32_t getHeight() const = 0;
	};
}

namespace FIFE {
	class Image;
	
	%feature("notabstract") GuiFont;
	class GuiFont : public fcn::Font, public IFont {
	public:
		GuiFont(IFont* font);
		virtual ~GuiFont();
		
		int32_t getStringIndexAt(const std::string& text, int32_t x);
		void drawString(fcn::Graphics* graphics, const std::string& text, int32_t x, int32_t y);

		void setRowSpacing (int32_t spacing);
		int32_t getRowSpacing() const;
		void setGlyphSpacing(int32_t spacing);
		int32_t getGlyphSpacing() const;
		void setAntiAlias(bool antiAlias);
		bool isAntiAlias();
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
		void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);
		SDL_Color getColor() const;
		int32_t getWidth(const std::string& text) const;
		int32_t getHeight() const;
	};
}
