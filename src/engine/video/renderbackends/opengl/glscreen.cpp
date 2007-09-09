/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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
#include "util/fifewin.h"

// 3rd party library includes
#include <guichan/opengl.hpp>
#include <GL/gl.h>
#include <SDL.h>

// FIFE includes
#include "video/rect.h"
#include "video/renderable.h"
#include "imagecache.h"

#include "glscreen.h"

#ifdef GUICHAN05
#include "video/gui/gcnfifeimage.h"
#endif

namespace FIFE {

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
#ifdef GUICHAN05
    const GCNImage* g_img = static_cast<const GCNImage*>(image);
		size_t imgid = g_img->_getData();
#else
		size_t imgid = reinterpret_cast<size_t>(image->_getData());
#endif
		RenderAble* fifeimg = ImageCache::instance()->getImage(imgid);

		const gcn::ClipRectangle& clip = getCurrentClipArea();
		Rect rect(dstX, dstY, width, height);
		rect.x += clip.xOffset;
		rect.y += clip.yOffset;

		fifeimg->render(rect, this);
	}

	void GLScreen::drawPoint(int x, int y) {
		glDisable(GL_TEXTURE_2D);
		m_gcn_graph->drawPoint(x, y);
		glEnable(GL_TEXTURE_2D);
	}

	void GLScreen::drawLine(int x1, int y1, int x2, int y2) {
		glDisable(GL_TEXTURE_2D);
		m_gcn_graph->drawLine(x1, y1, x2, y2);
		glEnable(GL_TEXTURE_2D);
	}

	void GLScreen::drawRectangle(const gcn::Rectangle& rectangle) {
		glDisable(GL_TEXTURE_2D);
		m_gcn_graph->drawRectangle(rectangle);
		glEnable(GL_TEXTURE_2D);
	}

	void GLScreen::fillRectangle(const gcn::Rectangle& rectangle) {
		glDisable(GL_TEXTURE_2D);
		m_gcn_graph->fillRectangle(rectangle);
		glEnable(GL_TEXTURE_2D);
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
