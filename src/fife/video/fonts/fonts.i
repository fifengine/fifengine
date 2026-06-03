// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "video/fonts/ifont.h"

%}
typedef uint8_t Uint8;

namespace FIFE {
	class Image;
	class IFont {
	public:
		virtual ~IFont();
		virtual void setRowSpacing (int32_t spacing) = 0;
		virtual int32_t getRowSpacing() const = 0;
		virtual void setGlyphSpacing(int32_t spacing) = 0;
		virtual int32_t getGlyphSpacing() const = 0;
		virtual void setAntiAlias(bool antiAlias) = 0;
		virtual bool isAntiAlias() = 0;
		virtual void setBoldStyle(bool style) = 0;
		virtual bool isBoldStyle() = 0;
		virtual void setItalicStyle(bool style) = 0;
		virtual bool isItalicStyle() = 0;
		virtual void setUnderlineStyle(bool style) = 0;
		virtual bool isUnderlineStyle() = 0;
		virtual void setStrikethroughStyle(bool style) = 0;
		virtual bool isStrikethroughStyle() = 0;
		virtual void setDynamicColoring(bool coloring) = 0;
		virtual bool isDynamicColoring() = 0;
		virtual void setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a = 255) = 0;
		virtual SDL_Color getColor() const = 0;
		virtual int32_t getWidth(const std::string& text) const = 0;
		virtual int getHeight() const = 0;
		virtual int32_t getStringIndexAt(const std::string& text, int32_t x) const = 0;
		virtual Image* getAsImage(const std::string& text) const = 0;
		virtual void invalidate() = 0;
	};

}