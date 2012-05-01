/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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

// This needs to be here, before fifechan includes gl.h
#include "video/opengl/fife_opengl.h"

// 3rd party library includes
#include <fifechan/opengl.hpp>
#include <fifechan/font.hpp>


// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "gui/fifechan/base/gui_image.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/opengl/renderbackendopengl.h"

#include "opengl_gui_graphics.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	OpenGLGuiGraphics::OpenGLGuiGraphics() {
		SDL_Surface* target = SDL_GetVideoSurface();
		assert(target);
		setTargetPlane(target->w, target->h);
		mColor = fcn::Color(255, 255, 255, 255);
		m_renderbackend = static_cast<RenderBackendOpenGL*>(RenderBackend::instance());
	}

	void OpenGLGuiGraphics::drawImage(const fcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height) {
		const GuiImage* g_img = dynamic_cast<const GuiImage*>(image);
		assert(g_img);

		ImagePtr fifeimg = g_img->getFIFEImage();
		const fcn::ClipRectangle& clip = mClipStack.top();
		fifeimg->render(Rect(dstX + clip.xOffset, dstY + clip.yOffset,
			width, height));
	}

	void OpenGLGuiGraphics::drawText(const std::string& text, int32_t x, int32_t y,
			uint32_t alignment) {
		if (mFont == NULL)
		{
			throw GuiException("OpenGLGuiGraphics::drawText() - No font set!");
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
				FL_WARN(_log, LMsg("OpenGLGuiGraphics::drawText() - ") << "Unknown alignment: " << alignment);
				mFont->drawString(this, text, x, y);
		}
	}

	void OpenGLGuiGraphics::drawPoint(int32_t x, int32_t y) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->putPixel(x + top.xOffset, y + top.yOffset,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void OpenGLGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		const fcn::ClipRectangle& top = mClipStack.top();
		x1 += top.xOffset;
		x2 += top.xOffset;
		y1 += top.yOffset;
		y2 += top.yOffset;

		Point pbegin(static_cast<int32_t>(ceil(x1 + 0.375f)), static_cast<int32_t>(ceil(y1 + 0.375f)));
		Point pend(static_cast<int32_t>(ceil(x2 + 0.625f)), static_cast<int32_t>(ceil(y2 + 0.625f)));

		m_renderbackend->drawLine(pbegin, pend,
			mColor.r, mColor.g, mColor.b, mColor.a);
		m_renderbackend->putPixel(pbegin.x, pbegin.y,
			mColor.r, mColor.g, mColor.b, mColor.a);
		m_renderbackend->putPixel(pend.x, pend.y,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void OpenGLGuiGraphics::drawRectangle(const fcn::Rectangle& rectangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawRectangle(
			Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
			rectangle.width, rectangle.height,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void OpenGLGuiGraphics::fillRectangle(const fcn::Rectangle& rectangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->fillRectangle(
			Point(rectangle.x + top.xOffset, rectangle.y + top.yOffset),
			rectangle.width, rectangle.height,
			mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void OpenGLGuiGraphics::_beginDraw() {
		fcn::Rectangle area(0, 0, mWidth, mHeight);
		fcn::Graphics::pushClipArea(area);
		m_renderbackend->pushClipArea(Rect(0, 0, mWidth, mHeight), false);
	}

	void OpenGLGuiGraphics::_endDraw() {
		m_renderbackend->renderVertexArrays();

		// Cleanup
		fcn::Graphics::popClipArea();
		m_renderbackend->popClipArea();
	}

	bool OpenGLGuiGraphics::pushClipArea(fcn::Rectangle area) {
		// Render what we gathered so far
		m_renderbackend->renderVertexArrays();
		fcn::Graphics::pushClipArea(area);

		// Due to some odd conception in guiChan some of area
		// has xOffset and yOffset > 0. And if it happens we
		// need to offset our clip area. Or we can use fifechan stack.
		const fcn::ClipRectangle& top = mClipStack.top();

		m_renderbackend->pushClipArea(
			Rect(top.x, top.y, top.width, top.height), false);

		return true;
	}

	void OpenGLGuiGraphics::popClipArea() {
		// Render what we gathered so far
		m_renderbackend->renderVertexArrays();
		fcn::Graphics::popClipArea();
		m_renderbackend->popClipArea();
	}

	void OpenGLGuiGraphics::setColor(const fcn::Color& color) {
		mColor = color;
	}
}
