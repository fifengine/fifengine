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
#include "video/devicecaps.h"

namespace FIFE {


	RenderBackend::RenderBackend(const SDL_Color& colorkey):
		m_screen(NULL),
		m_isalphaoptimized(false),
		m_iscolorkeyenabled(false),
		m_colorkey(colorkey),
		m_isframelimit(false),
		m_framelimit(60) {
	}


	RenderBackend::~RenderBackend() {
	}

	void RenderBackend::deinit() {
		delete m_screen;
		m_screen = NULL;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	void RenderBackend::startFrame() {
		if (m_isframelimit) {
			m_frame_start = SDL_GetTicks();
		}
	}

	void RenderBackend::endFrame () {
		if (m_isframelimit) {
			uint16_t frame_time = SDL_GetTicks() - m_frame_start;
			const float frame_limit = 1000.0f/m_framelimit;
			if (frame_time < frame_limit) {
				SDL_Delay(static_cast<Uint32>(frame_limit) - frame_time);
			}
		}
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

	void RenderBackend::setSurface(SDL_Surface* surface) {
		assert(m_screen);
		m_screen->setSurface(surface);
	}

	const ScreenMode& RenderBackend::getCurrentScreenMode() const{
		return m_screenMode;
	}

	uint32_t RenderBackend::getWidth() const {
		assert(m_screen);
		return m_screen->getWidth();
	}

	uint32_t RenderBackend::getHeight() const {
		assert(m_screen);
		return m_screen->getHeight();
	}

	const Rect& RenderBackend::getArea() {
		assert(m_screen);
		SDL_Surface* s = m_screen->getSurface();
		static Rect r(0, 0, s->w, s->h);
		return r;
	}

	void RenderBackend::getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
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

	void RenderBackend::setColorKeyEnabled(bool colorkeyenable) {
		m_iscolorkeyenabled = colorkeyenable;
	}

	bool RenderBackend::isColorKeyEnabled() const {
		return m_iscolorkeyenabled;
	}

	void RenderBackend::setColorKey(const SDL_Color& colorkey) {
		m_colorkey = colorkey;
	}

	const SDL_Color& RenderBackend::getColorKey() const {
		return m_colorkey;
	}

	void RenderBackend::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
		assert(m_screen);
		m_screen->setBackgroundColor(r,g,b);
	}

	void RenderBackend::resetBackgroundColor() {
		assert(m_screen);
		m_screen->setBackgroundColor(0,0,0);
	}
	
	const SDL_PixelFormat& RenderBackend::getPixelFormat() const {
		return m_rgba_format;
	}

	void RenderBackend::setFrameLimitEnabled(bool limited) {
		m_isframelimit = limited;
	}

	bool RenderBackend::isFrameLimitEnabled() const {
		return m_isframelimit;
	}

	void RenderBackend::setFrameLimit(uint16_t framelimit) {
		m_framelimit = framelimit;
	}

	uint16_t RenderBackend::getFrameLimit() const {
		return m_framelimit;
	}
}
