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

// This needs to be here, before guichan includes gl.h
#include "video/opengl/fife_opengl.h"

// 3rd party library includes
#include <guichan/opengl.hpp>
#include <guichan/font.hpp>


// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src dir
#include "util/log/logger.h"
#include "util/base/exception.h"
#include "gui/guichan/base/gui_image.h"
#include "util/structures/rect.h"
#include "video/image.h"
#include "video/imagemanager.h"
#include "video/renderbackend.h"
#include "video/opengl/renderbackendopengl.h"

#include "opengl_gui_graphics.h"

namespace FIFE {
	static Logger _log(LM_GUI);

	OpenGLGuiGraphics::OpenGLGuiGraphics() {
		mTarget = SDL_GetVideoSurface();
		assert(mTarget);
		setTargetPlane(mTarget->w, mTarget->h);

	}

	void OpenGLGuiGraphics::drawImage(const gcn::Image* image, int32_t srcX, int32_t srcY, int32_t dstX, int32_t dstY, int32_t width, int32_t height) {
		const GuiImage* g_img = dynamic_cast<const GuiImage*>(image);
		assert(g_img);

		ImagePtr fifeimg = g_img->getFIFEImage();
		const gcn::ClipRectangle& clip = getCurrentClipArea();
		Rect rect(dstX, dstY, width, height);
		rect.x += clip.xOffset;
		rect.y += clip.yOffset;

		fifeimg->render(rect, mTarget);
		RenderBackend::instance()->renderVertexArrays();
	}

	void OpenGLGuiGraphics::drawText(const std::string& text, int32_t x, int32_t y,
			uint32_t alignment) {
		if (mFont == NULL)
		{
			throw GuiException("OpenGLGuiGraphics::drawText() - No font set!");
		}

		reinterpret_cast<RenderBackendOpenGL*>(RenderBackend::instance())->enableTextures(0);
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
				FL_WARN(_log, LMsg("OpenGLGuiGraphics::drawText() - ") << "Unknown alignment: " << alignment);
				mFont->drawString(this, text, x, y);
		}
	}

	void OpenGLGuiGraphics::drawPoint(int32_t x, int32_t y) {
		reinterpret_cast<RenderBackendOpenGL*>(RenderBackend::instance())->disableTextures(0);
		gcn::OpenGLGraphics::drawPoint(x, y);
	}

	void OpenGLGuiGraphics::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
		reinterpret_cast<RenderBackendOpenGL*>(RenderBackend::instance())->disableTextures(0);
		gcn::OpenGLGraphics::drawLine(x1, y1, x2, y2);
	}

	void OpenGLGuiGraphics::drawRectangle(const gcn::Rectangle& rectangle) {
		reinterpret_cast<RenderBackendOpenGL*>(RenderBackend::instance())->disableTextures(0);
		gcn::OpenGLGraphics::drawRectangle(rectangle);
	}
}
