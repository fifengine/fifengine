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

%module(directors="1") fonts
%{
#include "gui/fonts/truetypefont.h"
#include "gui/fonts/subimagefont.h"

%}
typedef unsigned char Uint8;

namespace gcn {
	class Font
	{
	public:
		virtual ~Font(){ }
		virtual int getWidth(const std::string& text) const = 0;
		virtual int getHeight() const = 0;
	};
}

namespace FIFE {
	class FontBase: public gcn::Font {
	public:
		virtual void setColor(uint8_t r,uint8_t g,uint8_t b) = 0;
	};

	%rename(TTFont) TrueTypeFont;
	class TrueTypeFont: public FIFE::FontBase {
	public:
		TrueTypeFont(const std::string& filename, int size);
		virtual ~TrueTypeFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
		virtual int getWidth(const std::string& text) const;
		virtual int getHeight() const;
	};

	class ImagePool;
	class SubImageFont: public FontBase {
		public:
			SubImageFont(const std::string& filename, const std::string& glyphs, ImagePool& pool);
			virtual ~SubImageFont();
			virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
			virtual int getWidth(const std::string& text) const;
			virtual int getHeight() const;
	};
}