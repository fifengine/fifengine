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
#include "video/fonts/ifont.h"
#include "video/fonts/fontbase.h"
#include "video/fonts/truetypefont.h"
#include "video/fonts/subimagefont.h"

%}
typedef uint8_t Uint8;

namespace FIFE {
	class IFont {
	public:
		virtual ~IFont();
		virtual void setRowSpacing (int32_t spacing) = 0;
		virtual int32_t getRowSpacing() const = 0;
		virtual void setGlyphSpacing(int32_t spacing) = 0;
		virtual int32_t getGlyphSpacing() const = 0;
		virtual void setAntiAlias(bool antiAlias) = 0;
		virtual bool isAntiAlias() = 0;
		virtual void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255) = 0;
		virtual SDL_Color getColor() const = 0;
		virtual int32_t getWidth(const std::string& text) const = 0;
		virtual int32_t getHeight() const = 0;
	};

	class FontBase: public IFont {
	public:
		virtual ~FontBase(){ }
	};

	%feature("notabstract") TrueTypeFont;
	%rename(TTFont) TrueTypeFont;
	class TrueTypeFont: public FontBase {
	public:
		TrueTypeFont(const std::string& filename, int32_t size);
		virtual ~TrueTypeFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
		virtual int32_t getWidth(const std::string& text) const;
		virtual int32_t getHeight() const;
	};

	%feature("notabstract") SubImageFont;
	class SubImageFont: public FontBase {
	public:
		SubImageFont(const std::string& filename, const std::string& glyphs);
		virtual ~SubImageFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
		virtual int32_t getWidth(const std::string& text) const;
		virtual int32_t getHeight() const;
	};
}
