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

#ifndef FIFE_GUI_OPENGL_GRAPHICS_H
#define FIFE_GUI_OPENGL_GRAPHICS_H

// Standard C++ library includes

// 3rd party library includes
#include <guichan/opengl/openglgraphics.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "video/renderablepool.h"

namespace FIFE {
	/** Overrides Guichan Graphics to enable usage of normal fife images & related facilities
	 */
	class OpenGLGuiGraphics: public gcn::OpenGLGraphics {
	public:
		/** Constructor
		 */
		OpenGLGuiGraphics(RenderablePool& pool);
		virtual void drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height);

		virtual void drawText(const std::string& text, int x, int y, unsigned int alignment);
		virtual void drawPoint(int x, int y);
		virtual void drawLine(int x1, int y1, int x2, int y2);
		virtual void drawRectangle(const gcn::Rectangle& rectangle);

	private:
		SDL_Surface* mTarget;
		RenderablePool& m_pool;
	};
}

#endif
