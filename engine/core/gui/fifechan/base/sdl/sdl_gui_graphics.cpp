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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "gui/fifechan/base/gui_image.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/sdl/renderbackendsdl.h"

#include "sdl_gui_graphics.h"

namespace FIFE {
	SdlGuiGraphics::SdlGuiGraphics() {
		setTarget(SDL_GetVideoSurface());
		m_renderbackend = static_cast<RenderBackendSDL*>(RenderBackend::instance());
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

	void SdlGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawThickLine(Point(x1+top.xOffset, y1+top.yOffset), Point(x2+top.xOffset, y2+top.yOffset), width, mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawPolyLine(const fcn::PointVector& points, uint32_t width) {
		const fcn::ClipRectangle& top = mClipStack.top();
		std::vector<Point> npoints;
		fcn::PointVector::const_iterator it = points.begin();
		for (; it != points.end(); ++it) {
			npoints.push_back(Point((*it).x+top.xOffset, (*it).y+top.yOffset));
		}
		m_renderbackend->drawPolyLine(npoints, width, mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawBezier(const fcn::PointVector& points, int32_t steps, uint32_t width) {
		const fcn::ClipRectangle& top = mClipStack.top();
		std::vector<Point> npoints;
		fcn::PointVector::const_iterator it = points.begin();
		for (; it != points.end(); ++it) {
			npoints.push_back(Point((*it).x+top.xOffset, (*it).y+top.yOffset));
		}
		m_renderbackend->drawBezier(npoints, steps, width, mColor.r, mColor.g, mColor.b, mColor.a);
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

	void SdlGuiGraphics::drawCircle(const fcn::Point& p, uint32_t radius) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawCircle(Point(p.x+top.xOffset, p.y+top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawFillCircle(const fcn::Point& p, uint32_t radius) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawFillCircle(Point(p.x+top.xOffset, p.y+top.yOffset), radius, mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawCircleSegment(Point(p.x+top.xOffset, p.y+top.yOffset), radius, sangle, eangle, mColor.r, mColor.g, mColor.b, mColor.a);
	}

	void SdlGuiGraphics::drawFillCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle) {
		const fcn::ClipRectangle& top = mClipStack.top();
		m_renderbackend->drawFillCircleSegment(Point(p.x+top.xOffset, p.y+top.yOffset), radius, sangle, eangle, mColor.r, mColor.g, mColor.b, mColor.a);
	}
}
