/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>
#include <fifechan/font.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "gui/fifechan/base/gui_image.h"
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "sdl_gui_graphics.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	SdlGuiGraphics::SdlGuiGraphics() {
		m_renderbackend = RenderBackend::instance();
		setTarget(m_renderbackend->getScreenSurface());
	}

	void SdlGuiGraphics::drawImage(const fcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height) {
		const GuiImage* g_img = dynamic_cast<const GuiImage*>(image);
		assert(g_img);

		ImagePtr fifeimg = g_img->getFIFEImage();
		const fcn::ClipRectangle& clip = getCurrentClipArea();
		Rect rect(dstX, dstY, width, height);
		rect.x += clip.xOffset;
		rect.y += clip.yOffset;

		fifeimg->render(rect);
	}

	void SdlGuiGraphics::drawText(const std::string& text, int32_t x, int32_t y,
			uint32_t alignment) {
		if (mFont == NULL)
		{
			throw GuiException("SdlGuiGraphics::drawText() - No font set!");
		}

		switch (alignment)
		{
			case Left:
				mFont->drawString(this, text, x, y);
				break;
			case Center:
				mFont->drawString(this, text, x - mFont->getWidth(text) / 2, y);
				break;
			case Right:
				mFont->drawString(this, text, x - mFont->getWidth(text), y);
				break;
			default:
				FL_WARN(_log, LMsg("SdlGuiGraphics::drawText() - ") << "Unknown alignment: " << alignment);
				mFont->drawString(this, text, x, y);
		}
	}

	void SdlGuiGraphics::drawPoint(int32_t x, int32_t y) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		const fcn::ClipRectangle& top = mClipStack.top();
		x1 += top.xOffset;
		x2 += top.xOffset;
		y1 += top.yOffset;
		y2 += top.yOffset;

		Point pbegin(x1, y1);
		Point pend(x2, y2);

		m_renderbackend->drawLine(pbegin, pend,
			mColor.r, mColor.g, mColor.b, mColor.a);
		m_renderbackend->putPixel(pbegin.x, pbegin.y,
			mColor.r, mColor.g, mColor.b, mColor.a);
		m_renderbackend->putPixel(pend.x, pend.y,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawRectangle(const fcn::Rectangle& rectangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawRectangle(
			Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
			rectangle.width, rectangle.height,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::fillRectangle(const fcn::Rectangle& rectangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->fillRectangle(
			Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
			rectangle.width, rectangle.height,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::_beginDraw() {
		fcn::Rectangle area(0, 0, mTarget->w, mTarget->h);
		fcn::Graphics::pushClipArea(area);
		m_renderbackend->pushClipArea(Rect(0, 0, mTarget->w, mTarget->h), false);
	}

	void SdlGuiGraphics::_endDraw() {
		// Cleanup
		fcn::Graphics::popClipArea();
		m_renderbackend->popClipArea();
	}

	bool SdlGuiGraphics::pushClipArea(fcn::Rectangle area) {
		fcn::Graphics::pushClipArea(area);

		// Due to some odd conception in FifeChan some of area
		// has xOffset and yOffset > 0. And if it happens we
		// need to offset our clip area. Or we can use fifechan stack.
		const fcn::ClipRectangle& top = mClipStack.top();

		m_renderbackend->pushClipArea(
			Rect(top.x, top.y, top.width, top.height), false);

		return true;
	}

	void SdlGuiGraphics::popClipArea() {
		fcn::Graphics::popClipArea();
		m_renderbackend->popClipArea();
	}

	void SdlGuiGraphics::setColor(const fcn::Color& color) {
		mColor = color;
	}
}
