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

#ifndef FIFE_GUI_OPENGL_GRAPHICS_H
#define FIFE_GUI_OPENGL_GRAPHICS_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/opengl/openglgraphics.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "video/imagepool.h"

namespace FIFE {
	/** Overrides Guichan Graphics to enable usage of normal fife images & related facilities
	 */
	class OpenGLGuiGraphics: public gcn::OpenGLGraphics {
	public:
		/** Constructor
		 */
		OpenGLGuiGraphics(ImagePool& pool);
		virtual void drawImage(const gcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height);

		virtual void drawText(const std::string& text, int32_t x, int32_t y, uint32_t alignment);
		virtual void drawPoint(int32_t x, int32_t y);
		virtual void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
		virtual void drawRectangle(const gcn::Rectangle& rectangle);

	private:
		SDL_Surface* mTarget;
		ImagePool& m_pool;
	};
}

#endif
