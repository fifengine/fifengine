/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
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
#include <guichan/font.hpp>
#include "gui/base/gui_font.h"
#include "video/fonts/abstractfont.h"
%}

%include "video/fonts/fonts.i"

namespace gcn {
	class Graphics;
	class Font
	{
	public:
		virtual ~Font(){ }
		virtual int getWidth(const std::string& text) const = 0;
		virtual int getHeight() const = 0;
	};
}

namespace FIFE {
	class Image;
	
	%feature("notabstract") GuiFont;
	class GuiFont : public gcn::Font, public AbstractFont {
	public:
		GuiFont(AbstractFont* font);
		virtual ~GuiFont();
		
		int getStringIndexAt(const std::string& text, int x);
		void drawString(gcn::Graphics* graphics, const std::string& text, int x, int y);

		void setRowSpacing (int spacing);
		int getRowSpacing() const;
		void setGlyphSpacing(int spacing);
		int getGlyphSpacing() const;
		void setAntiAlias(bool antiAlias);
		bool isAntiAlias();
		Image* getAsImage(const std::string& text);
		void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255);
		SDL_Color getColor() const;
		int getWidth(const std::string& text) const;
		int getHeight() const;
	};
}
