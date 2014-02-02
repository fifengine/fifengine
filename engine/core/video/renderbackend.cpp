/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
		m_target(NULL),
		m_compressimages(false),
		m_useframebuffer(false),
		m_usenpot(false),
		m_isalphaoptimized(false),
		m_iscolorkeyenabled(false),
		m_colorkey(colorkey),
		m_isMipmapping(false),
		m_textureFilter(TEXTURE_FILTER_NONE),
		m_maxAnisotropy(0),
		m_monochrome(false),
		m_isDepthBuffer(false),
		m_alphaValue(0.3),
		m_isframelimit(false),
		m_framelimit(60) {

		m_isbackgroundcolor = false;
		m_backgroundcolor.r = 0;
		m_backgroundcolor.g = 0;
		m_backgroundcolor.b = 0;
	}

	RenderBackend::~RenderBackend() {
	}

	void RenderBackend::deinit() {
		//delete m_screen;
		//m_screen = NULL;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		SDL_Quit();
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

	const ScreenMode& RenderBackend::getCurrentScreenMode() const{
		return m_screenMode;
	}

	uint32_t RenderBackend::getWidth() const {
		return m_screen->w;
	}

	uint32_t RenderBackend::getHeight() const {
		return m_screen->h;
	}

	const Rect& RenderBackend::getArea() const {
		static Rect r(0, 0, m_screen->w, m_screen->h);
		return r;
	}

	void RenderBackend::pushClipArea(const Rect& cliparea, bool clear) {
		ClipInfo ci;
		ci.r = cliparea;
		ci.clearing = clear;
		m_clipstack.push(ci);
		setClipArea(cliparea, clear);
	}

	void RenderBackend::popClipArea() {
		assert(!m_clipstack.empty());
		m_clipstack.pop();
		if (m_clipstack.empty()) {
			setClipArea(getArea(), false);
		} else {
			ClipInfo ci = m_clipstack.top();
			// instead of using ci.clearing, we set it to false
			// to avoid double clearing
			setClipArea(ci.r, false);
		}
	}

	const Rect& RenderBackend::getClipArea() const {
		if (!m_clipstack.empty()) {
			return m_clipstack.top().r;
		} else {
			return getArea();
		}
	}

	void RenderBackend::clearClipArea() {
		setClipArea(getArea(), true);
	}

	void RenderBackend::setTextureFiltering(TextureFiltering filter) {
		m_textureFilter = filter;
	}

	TextureFiltering RenderBackend::getTextureFiltering() const {
		return m_textureFilter;
	}

	void RenderBackend::setMipmappingEnabled(bool enabled) {
		m_isMipmapping = enabled;
	}

	bool RenderBackend::isMipmappingEnabled() const {
		return m_isMipmapping;
	}
	
	int32_t RenderBackend::getMaxAnisotropy() const {
		return m_maxAnisotropy;
	}
	
	void RenderBackend::setMonochromeEnabled(bool enabled) {
		m_monochrome = enabled;
	}

	bool RenderBackend::isMonochromeEnabled() const {
		return m_monochrome;
	}

	void RenderBackend::setDepthBufferEnabled(bool enabled) {
		m_isDepthBuffer = enabled;
	}

	bool RenderBackend::isDepthBufferEnabled() const {
		return m_isDepthBuffer;
	}

	void RenderBackend::setAlphaTestValue(float alpha) {
		m_alphaValue = alpha;
	}

	float RenderBackend::getAlphaTestValue() const {
		return m_alphaValue;
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
		if (r != m_backgroundcolor.r || g != m_backgroundcolor.g || b != m_backgroundcolor.b) {
			m_isbackgroundcolor = true;
			m_backgroundcolor.r = r;
			m_backgroundcolor.g = g;
			m_backgroundcolor.b = b;
		}
	}

	void RenderBackend::resetBackgroundColor() {
		setBackgroundColor(0,0,0);
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

	SDL_Surface* RenderBackend::getRenderTargetSurface() {
		return m_target;
	}
}
