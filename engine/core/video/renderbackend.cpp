/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderbackend.h"

namespace FIFE {
	
	RenderBackend::RenderBackend(): 
		m_screen(NULL), m_isalphaoptimized(false) {
	}


	RenderBackend::~RenderBackend() {
	}

	void RenderBackend::deinit() {
		delete m_screen;
		m_screen = NULL;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	void RenderBackend::captureScreen(const std::string& filename) {
		m_screen->saveImage(filename);
	}
	
	void RenderBackend::pushClipArea(const Rect& cliparea, bool clear) {
		assert(m_screen);
		m_screen->pushClipArea(cliparea, clear);
        }
	
	void RenderBackend::popClipArea() {
		assert(m_screen);
		m_screen->popClipArea();
	}
	
	const Rect& RenderBackend::getClipArea() const {
		assert(m_screen);
		return m_screen->getClipArea();
	}
	
	SDL_Surface* RenderBackend::getSurface() {
		assert(m_screen);
		return m_screen->getSurface();
	}
	
	unsigned int RenderBackend::getWidth() const {
		assert(m_screen);
		return m_screen->getWidth();
	}
	
	unsigned int RenderBackend::getHeight() const {
		assert(m_screen);
		return m_screen->getHeight();
	}
	
	const Rect& RenderBackend::getArea() {
		assert(m_screen);
		return m_screen->getArea();
	}
	
	void RenderBackend::getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		assert(m_screen);
		m_screen->getPixelRGBA(x, y, r, g, b, a);
	}
	
	bool RenderBackend::putPixel(int x, int y, int r, int g, int b) {
		assert(m_screen);
		return m_screen->putPixel(x, y, r, g, b);
	}
	
	void RenderBackend::drawLine(const Point& p1, const Point& p2, int r, int g, int b) {
		assert(m_screen);
		m_screen->drawLine(p1, p2, r, g, b);
	}
	
	void RenderBackend::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b) {
		assert(m_screen);
		m_screen->drawQuad(p1, p2, p3, p4, r, g, b);
	}
	
	void RenderBackend::saveImage(const std::string& filename) {
		assert(m_screen);
		m_screen->saveImage(filename);
	}
	
	void RenderBackend::setAlphaOptimizerEnabled(bool enabled) {
		assert(m_screen);
		m_screen->setAlphaOptimizerEnabled(enabled);
	}
	
	bool RenderBackend::isAlphaOptimizerEnabled() {
		assert(m_screen);
		return m_screen->isAlphaOptimizerEnabled();
	}
	
}
