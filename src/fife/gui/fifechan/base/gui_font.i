// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

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