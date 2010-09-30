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
#include <guichan/opengl.hpp>
#include <guichan/font.hpp>
#include <guichan/exception.hpp>


// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "video/image.h"
#include "gui/base/gui_image.h"
#include "util/structures/rect.h"
#include "video/opengl/fife_opengl.h"

#include "opengl_gui_graphics.h"

namespace FIFE {
	OpenGLGuiGraphics::OpenGLGuiGraphics(ImagePool& pool): m_pool(pool) {
		mTarget = SDL_GetVideoSurface();
		assert(mTarget);
		setTargetPlane(mTarget->w, mTarget->h);

	}

	void OpenGLGuiGraphics::drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height) {
		const GuiImage* g_img = dynamic_cast<const GuiImage*>(image);
		assert(g_img);
		Image& fifeimg = m_pool.getImage(g_img->getPoolId());
		const gcn::ClipRectangle& clip = getCurrentClipArea();
		Rect rect(dstX, dstY, width, height);
		rect.x += clip.xOffset;
		rect.y += clip.yOffset;
		GLEnable flag(GL_TEXTURE_2D);
		fifeimg.render(rect, mTarget);
	}

	void OpenGLGuiGraphics::drawText(const std::string& text, int x, int y,
			unsigned int alignment) {
		if (mFont == NULL)
		{
			throw GCN_EXCEPTION("No font set.");
		}

		GLEnable flag(GL_TEXTURE_2D);
		switch (alignment)
		{
			case LEFT:
				mFont->drawString(this, text, x, y);
				break;
			case CENTER:
				mFont->drawString(this, text, x - mFont->getWidth(text) / 2, y);
				break;
			case RIGHT:
				mFont->drawString(this, text, x - mFont->getWidth(text), y);
				break;
			default:
				throw GCN_EXCEPTION("Unknown alignment.");
		}
	}

	void OpenGLGuiGraphics::drawPoint(int x, int y) {
		GLDisable flag(GL_TEXTURE_2D);
		gcn::OpenGLGraphics::drawPoint(x, y);
	}

	void OpenGLGuiGraphics::drawLine(int x1, int y1, int x2, int y2) {
		GLDisable flag(GL_TEXTURE_2D);
		gcn::OpenGLGraphics::drawLine(x1, y1, x2, y2);
	}

	void OpenGLGuiGraphics::drawRectangle(const gcn::Rectangle& rectangle) {
		GLDisable flag(GL_TEXTURE_2D);
		gcn::OpenGLGraphics::drawRectangle(rectangle);
	}
}
