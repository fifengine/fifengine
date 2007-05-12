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

// Standard C++ library includes

// Platform specific includes

// 3rd party library includes
#include <guichan/opengl.hpp>
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"
#include "video/renderable.h"
#include "video/imagecache.h"

#include "fife_opengl.h"
#include "glscreen.h"

#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "video/gui/gcnfifeimage.h"
#include "guichan/font.hpp"
#include "guichan/exception.hpp"
#endif

namespace FIFE {

	struct GLEnable {
		GLenum m_flag;
		GLboolean m_oldval;
		GLEnable(GLenum flag) : m_flag(flag) { 
			glGetBooleanv(flag, &m_oldval);
			if (!m_oldval) {
				glEnable(flag);
			}
		}
		~GLEnable() { 
			if (!m_oldval) {
				glDisable(m_flag);
			}
		}
	};

	struct GLDisable {
		GLenum m_flag;
		GLboolean m_oldval;
		GLDisable(GLenum flag) : m_flag(flag) { 
			glGetBooleanv(flag, &m_oldval);
			if (m_oldval) {
				glDisable(flag);
			}
		}
		~GLDisable() { 
			if (m_oldval) {
				glEnable(m_flag);
			}
		}
	};


	GLScreen::GLScreen() : m_gcn_graph(new gcn::OpenGLGraphics()) {
		m_gcn_graph->setTargetPlane(SDL_GetVideoSurface()->w, SDL_GetVideoSurface()->h);
	}

	GLScreen::~GLScreen() {
		delete m_gcn_graph;
	}

	unsigned int GLScreen::getWidth() const {
		return SDL_GetVideoSurface()->w;
	}

	unsigned int GLScreen::getHeight() const {
		return SDL_GetVideoSurface()->h;
	}

	void GLScreen::drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height) {
#if GUICHAN_VERSION == 4
		size_t imgid = reinterpret_cast<size_t>(image->_getData());
#else
		const GCNImage* g_img = static_cast<const GCNImage*>(image);
		size_t imgid = g_img->_getData();
#endif
		RenderAble* fifeimg = ImageCache::instance()->getImage(imgid);

		const gcn::ClipRectangle& clip = getCurrentClipArea();
		Rect rect(dstX, dstY, width, height);
		rect.x += clip.xOffset;
		rect.y += clip.yOffset;

#if GUICHAN_VERSION != 4
		GLEnable flag(GL_TEXTURE_2D);
#endif
		fifeimg->render(rect, this);
	}

#if GUICHAN_VERSION != 4
	void GLScreen::drawText(const std::string& text, int x, int y,
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
#endif

	void GLScreen::drawPoint(int x, int y) {
		GLDisable flag(GL_TEXTURE_2D);
		m_gcn_graph->drawPoint(x, y);
	}

	void GLScreen::drawLine(int x1, int y1, int x2, int y2) {
		GLDisable flag(GL_TEXTURE_2D);
		m_gcn_graph->drawLine(x1, y1, x2, y2);
	}

	void GLScreen::drawRectangle(const gcn::Rectangle& rectangle) {
		GLDisable flag(GL_TEXTURE_2D);
		m_gcn_graph->drawRectangle(rectangle);
	}

	void GLScreen::fillRectangle(const gcn::Rectangle& rectangle) {
		GLDisable flag(GL_TEXTURE_2D);
		m_gcn_graph->fillRectangle(rectangle);
	}

	void GLScreen::setColor(const gcn::Color& color) {
		m_gcn_graph->setColor(color);
	}

	const gcn::Color& GLScreen::getColor() {
		return m_gcn_graph->getColor();
	}

	void GLScreen::_beginDraw() {
		m_gcn_graph->_beginDraw();
	}

	void GLScreen::_endDraw() {
		m_gcn_graph->_endDraw();
	}

	bool GLScreen::pushClipArea(gcn::Rectangle area) {
		return m_gcn_graph->pushClipArea(area);
	}

	void GLScreen::popClipArea() {
		m_gcn_graph->popClipArea();
	}

	const gcn::ClipRectangle& GLScreen::getCurrentClipArea() {
		return m_gcn_graph->getCurrentClipArea();
	}
}
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
