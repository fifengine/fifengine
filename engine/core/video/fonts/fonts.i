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

%module fife
%{
#include "video/fonts/abstractfont.h"
#include "video/fonts/fontbase.h"
#include "video/fonts/truetypefont.h"
#include "video/fonts/subimagefont.h"

%}
typedef unsigned char Uint8;

namespace FIFE {
	class AbstractFont {
	public:
		virtual ~AbstractFont();
		virtual void setRowSpacing (int spacing) = 0;
		virtual int getRowSpacing() const = 0;
		virtual void setGlyphSpacing(int spacing) = 0;
		virtual int getGlyphSpacing() const = 0;
		virtual void setAntiAlias(bool antiAlias) = 0;
		virtual bool isAntiAlias() = 0;
		virtual void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255) = 0;
		virtual SDL_Color getColor() const = 0;
		virtual int getWidth(const std::string& text) const = 0;
		virtual int getHeight() const = 0;
	};

	class FontBase: public AbstractFont {
	public:
		virtual ~FontBase(){ }
	};

	%feature("notabstract") TrueTypeFont;
	%rename(TTFont) TrueTypeFont;
	class TrueTypeFont: public FontBase {
	public:
		TrueTypeFont(const std::string& filename, int size);
		virtual ~TrueTypeFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
		virtual int getWidth(const std::string& text) const;
		virtual int getHeight() const;
	};

	class ImagePool;
	%feature("notabstract") SubImageFont;
	class SubImageFont: public FontBase {
	public:
		SubImageFont(const std::string& filename, const std::string& glyphs, ImagePool& pool);
		virtual ~SubImageFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
		virtual int getWidth(const std::string& text) const;
		virtual int getHeight() const;
	};
}
