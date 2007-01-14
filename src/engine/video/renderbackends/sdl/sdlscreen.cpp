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

// 3rd party library includes
#include <guichan/sdl.hpp>

// FIFE includes
#include "video/rect.h"
#include "video/renderable.h"
#include "imagecache.h"

#include "sdlscreen.h"

#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
#include "video/gui/gcnfifeimage.h"
#endif

namespace FIFE {

	SDLScreen::SDLScreen(SDL_Surface* screen) : m_screen(screen), m_gcn_graph(new gcn::SDLGraphics()) {
		m_gcn_graph->setTarget(m_screen);
	}

	SDLScreen::~SDLScreen() {
		delete m_gcn_graph;
	}

	SDL_Surface* SDLScreen::getSurface() const {
		return m_screen;
	}

	unsigned int SDLScreen::getWidth() const {
		return m_screen->w;
	}

	unsigned int SDLScreen::getHeight() const {
		return m_screen->h;
	}


  void SDLScreen::drawImage(const gcn::Image* image, int srcX, int srcY, int dstX, int dstY, int width, int height) {
#if GUICHAN_VERSION == 5 || GUICHAN_VERSION == 6
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

	void SDLScreen::drawPoint(int x, int y) {
		m_gcn_graph->drawPoint(x, y);
	}

	void SDLScreen::drawLine(int x1, int y1, int x2, int y2) {
		m_gcn_graph->drawLine(x1, y1, x2, y2);
	}

	void SDLScreen::drawRectangle(const gcn::Rectangle& rectangle) {
		m_gcn_graph->drawRectangle(rectangle);
	}

	void SDLScreen::fillRectangle(const gcn::Rectangle& rectangle) {
		m_gcn_graph->fillRectangle(rectangle);
	}

	void SDLScreen::setColor(const gcn::Color& color) {
		m_gcn_graph->setColor(color);
	}

	const gcn::Color& SDLScreen::getColor() {
		return m_gcn_graph->getColor();
	}

	void SDLScreen::_beginDraw() {
		m_gcn_graph->_beginDraw();
	}

	void SDLScreen::_endDraw() {
		m_gcn_graph->_endDraw();
	}

	bool SDLScreen::pushClipArea(gcn::Rectangle area) {
		return m_gcn_graph->pushClipArea(area);
	}

	void SDLScreen::popClipArea() {
		m_gcn_graph->popClipArea();
	}

	const gcn::ClipRectangle& SDLScreen::getCurrentClipArea() {
		return m_gcn_graph->getCurrentClipArea();
	}
}

