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

// Standard C++ library includes

// 3rd party library includes
#include <guichan.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "gui_font.h"

namespace FIFE {
	GuiFont::GuiFont(AbstractFont* font): m_font(font) {
		assert(font);
	}

	GuiFont::~GuiFont() {
		delete m_font;
	}

	int GuiFont::getStringIndexAt(const std::string& text, int x) const {
		return m_font->getStringIndexAt(text, x);
	}

	void GuiFont::drawString(gcn::Graphics* graphics, const std::string& text, int x, int y) {
		if (text == "") {
			return;
		}

		int yoffset = getRowSpacing() / 2;

		const gcn::ClipRectangle& clip = graphics->getCurrentClipArea();
		FIFE::Rect rect;
		rect.x = x + clip.xOffset;
		rect.y = y + clip.yOffset + yoffset;
		rect.w = getWidth(text);
		rect.h = getHeight();

		if (!rect.intersects(Rect(clip.x,clip.y,clip.width,clip.height)) ) {
			return;
		}

		Image* image = getAsImage(text);
		image->render(rect);
	}

	void GuiFont::drawMultiLineString(gcn::Graphics* graphics, const std::string& text, int x, int y) {
		if (text == "") {
			return;
		}

		int yoffset = getRowSpacing() / 2;

		const gcn::ClipRectangle& clip = graphics->getCurrentClipArea();

		Image* image = getAsImageMultiline(text);

		FIFE::Rect rect;
		rect.x = x + clip.xOffset;
		rect.y = y + clip.yOffset + yoffset;
		rect.w = image->getWidth();
		rect.h = image->getHeight();
		if (!rect.intersects(Rect(clip.x,clip.y,clip.width,clip.height)) ) {
			return;
		}
		image->render(rect);
	}

	void GuiFont::setRowSpacing (int spacing) {
		m_font->setRowSpacing(spacing);
	}

	int GuiFont::getRowSpacing() const {
		return m_font->getRowSpacing();
	}

	void GuiFont::setGlyphSpacing(int spacing) {
		m_font->setGlyphSpacing(spacing);
	}

	int GuiFont::getGlyphSpacing() const {
		return m_font->getGlyphSpacing();
	}

	void GuiFont::setAntiAlias(bool antiAlias) {
		m_font->setAntiAlias(antiAlias);
	}

	bool GuiFont::isAntiAlias() {
		return m_font->isAntiAlias();
	}

	Image* GuiFont::getAsImage(const std::string& text) {
		return m_font->getAsImage(text);
	}

	Image* GuiFont::getAsImageMultiline(const std::string& text) {
		return m_font->getAsImageMultiline(text);
	}

	std::string GuiFont::splitTextToWidth (const std::string& text, int render_width) {
		return m_font->splitTextToWidth(text,render_width);
	}

	void GuiFont::setColor(uint8_t r,uint8_t g,uint8_t b, uint8_t a) {
		m_font->setColor(r, g, b, a);
	}

	SDL_Color GuiFont::getColor() const {
		return m_font->getColor();
	}

	int GuiFont::getWidth(const std::string& text) const {
		return m_font->getWidth(text);
	}

	int GuiFont::getHeight() const {
		return m_font->getHeight();
	}

	void GuiFont::invalidate() {
		m_font->invalidate();
	}
}
