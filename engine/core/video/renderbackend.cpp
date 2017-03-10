/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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
		m_window(NULL),
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
		m_vSync(false),
		m_isframelimit(false),
		m_frame_start(0),
		m_framelimit(60) {

		m_isbackgroundcolor = false;
		m_backgroundcolor.r = 0;
		m_backgroundcolor.g = 0;
		m_backgroundcolor.b = 0;
	}

	RenderBackend::~RenderBackend() {
	}

	void RenderBackend::deinit() {
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

	void RenderBackend::setVSyncEnabled(bool vsync) {
		m_vSync = vsync;
	}

	bool RenderBackend::isVSyncEnabled() const {
		return m_vSync;
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

	SDL_Surface* RenderBackend::getScreenSurface() {
		return m_screen;
	}

	SDL_Surface* RenderBackend::getRenderTargetSurface() {
		return m_target;
	}

	Point RenderBackend::getBezierPoint(const std::vector<Point>& points, int32_t elements, float t) {
		if (t < 0.0) {
			return points[0];
		} else if (t >= static_cast<double>(elements)) {
			return points.back();
		}

		// Interpolate
		double px = 0.0;
		double py = 0.0;
		int32_t n = elements - 1;
		double muk = 1.0;
		double mu = static_cast<double>(t) / static_cast<double>(elements);
		double munk = Mathd::Pow(1.0 - mu, static_cast<double>(n));
		for (int32_t i = 0; i <= n; ++i) {
			int32_t tmpn = n;
			int32_t tmpi = i;
			int32_t diffn = n - i;
			double blend = muk * munk;
			muk *= mu;
			munk /= 1.0 - mu;
			while (tmpn) {
				blend *= static_cast<double>(tmpn);
				tmpn--;
				if (tmpi > 1) {
					blend /= static_cast<double>(tmpi);
					tmpi--;
				}
				if (diffn > 1) {
					blend /= static_cast<double>(diffn);
					diffn--;
				}
			}
			px += static_cast<double>(points[i].x) * blend;
			py += static_cast<double>(points[i].y) * blend;
		}

		return Point(static_cast<int32_t>(px), static_cast<int32_t>(py));
	}

	void RenderBackend::addControlPoints(const std::vector<Point>& points, std::vector<Point>& newPoints) {
		if (points.empty()) {
			return;
		}

		int32_t n = points.size() - 1;
		// min 2 points
		if (n < 1) {
			return;
		}

		Point p;
		// straight line
		if (n == 1) {
			newPoints.push_back(points[0]);
			p.x = (2 * points[0].x + points[1].x) / 3;
			p.y = (2 * points[0].y + points[1].y) / 3;
			newPoints.push_back(p);
			p.x = 2 * p.x - points[0].x;
			p.y = 2 * p.y - points[0].y;
			newPoints.push_back(p);
			newPoints.push_back(points[1]);
			return;
		}

		// calculate x and y values
		float* xrhs = new float[n];
		float* yrhs = new float[n];
		// first
		xrhs[0] = points[0].x + 2 * points[1].x;
		yrhs[0] = points[0].y + 2 * points[1].y;
		// last
		xrhs[n - 1] = (8 * points[n - 1].x + points[n].x) / 2.0;
		yrhs[n - 1] = (8 * points[n - 1].y + points[n].y) / 2.0;
		// rest
		for (int32_t i = 1; i < n - 1; ++i) {
			xrhs[i] = 4 * points[i].x + 2 * points[i + 1].x;
			yrhs[i] = 4 * points[i].y + 2 * points[i + 1].y;
		}

		float* x = new float[n];
		float* y = new float[n];
		float* xtmp = new float[n];
		float* ytmp = new float[n];
		float xb = 2.0;
		float yb = 2.0;
		x[0] = xrhs[0] / xb;
		y[0] = yrhs[0] / yb;
		// Decomposition and forward substitution.
		for (int32_t i = 1; i < n; i++) {
			xtmp[i] = 1 / xb;
			ytmp[i] = 1 / yb;
			xb = (i < n - 1 ? 4.0 : 3.5) - xtmp[i];
			yb = (i < n - 1 ? 4.0 : 3.5) - ytmp[i];
			x[i] = (xrhs[i] - x[i - 1]) / xb;
			y[i] = (yrhs[i] - y[i - 1]) / yb;
		}
		// Backward substitution
		for (int32_t i = 1; i < n; i++) {
			x[n - i - 1] -= xtmp[n - i] * x[n - i];
			y[n - i - 1] -= ytmp[n - i] * y[n - i];
		}

		// start point
		newPoints.push_back(points[0]);
		for (int32_t i = 0; i < n - 1; ++i) {
			p.x = x[i];
			p.y = y[i];
			newPoints.push_back(p);
			p.x = 2 * points[i + 1].x - x[i + 1];
			p.y = 2 * points[i + 1].y - y[i + 1];
			newPoints.push_back(p);

			newPoints.push_back(points[i+1]);
		}
		p.x = x[n - 1];
		p.y = y[n - 1];
		newPoints.push_back(p);
		p.x = (points[n].x + x[n - 1]) / 2;
		p.y = (points[n].y + y[n - 1]) / 2;
		newPoints.push_back(p);
		// end point
		newPoints.push_back(points[n]);

		delete[] xrhs;
		delete[] yrhs;
		delete[] x;
		delete[] y;
		delete[] xtmp;
		delete[] ytmp;
	}
}
