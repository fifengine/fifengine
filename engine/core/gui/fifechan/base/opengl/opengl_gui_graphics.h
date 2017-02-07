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

#ifndef FIFE_GUI_OPENGL_GRAPHICS_H
#define FIFE_GUI_OPENGL_GRAPHICS_H

// Standard C++ library includes

// 3rd party library includes
#include <fifechan/opengl/openglgraphics.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir

namespace FIFE {
	class RenderBackendOpenGL;

	/** Overrides Guichan Graphics to enable usage of normal fife images & related facilities
	 */
	class OpenGLGuiGraphics: public fcn::OpenGLGraphics {
	public:
		/** Constructor
		 */
		OpenGLGuiGraphics();
		virtual void drawImage(const fcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height);

		virtual void drawText(const std::string& text, int32_t x, int32_t y, uint32_t alignment);
		virtual void drawPoint(int32_t x, int32_t y);
		virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
		virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t width);
		virtual void drawPolyLine(const fcn::PointVector& points, uint32_t width);
		virtual void drawBezier(const fcn::PointVector& points, int32_t steps, uint32_t width);
		virtual void drawRectangle(const fcn::Rectangle& rectangle);
		virtual void fillRectangle(const fcn::Rectangle& rectangle);
		virtual void drawCircle(const fcn::Point& p, uint32_t radius);
		virtual void drawFillCircle(const fcn::Point& p, uint32_t radius);
		virtual void drawCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle);
		virtual void drawFillCircleSegment(const fcn::Point& p, uint32_t radius, int32_t sangle, int32_t eangle);

		virtual void _beginDraw();
		virtual void _endDraw();

		virtual bool pushClipArea(fcn::Rectangle area);
		virtual void popClipArea();

		virtual void setColor(const fcn::Color& color);

	private:
		RenderBackendOpenGL* m_renderbackend;
	};
}

#endif
