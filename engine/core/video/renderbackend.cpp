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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "renderbackend.h"

namespace FIFE {
	
	const unsigned int DEFAULT_CHUNKING_SIZE = 256;
	const unsigned int MAX_CHUNKING_SIZE = 262144;  // pixels!
	
	RenderBackend::RenderBackend(): 
		m_screen(NULL), m_isalphaoptimized(false), m_chunkingsize(DEFAULT_CHUNKING_SIZE) {
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
		SDL_Surface* s = m_screen->getSurface();
		static Rect r(0, 0, s->w, s->h);
		return r;
	}
	
	void RenderBackend::getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		assert(m_screen);
		m_screen->getPixelRGBA(x, y, r, g, b, a);
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
	
	void RenderBackend::setChunkingSize(unsigned int size) {
		if (size > MAX_CHUNKING_SIZE) {
			size = MAX_CHUNKING_SIZE;
		}
		m_chunkingsize = 1;
		while (m_chunkingsize < size) {
			m_chunkingsize <<= 1;
		}
	}
	
	unsigned int RenderBackend::getChunkingSize() {
		return m_chunkingsize;
	}
}
