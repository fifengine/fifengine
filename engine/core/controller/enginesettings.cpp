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
#include <algorithm>
#include <string>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"

#include "enginesettings.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_CONTROLLER);

	const float MAXIMUM_VOLUME = 10.0;

	EngineSettings::EngineSettings():
		m_bitsperpixel(0),
		m_fullscreen(false),
		m_refreshRate(60),
		m_displayIndex(0),
		m_vSync(false),
		m_renderDriver(""),
		m_initialvolume(MAXIMUM_VOLUME / 2),
		m_renderbackend("SDL"),
		m_sdlremovefakealpha(false),
		m_oglcompressimages(false),
		m_ogluseframebuffer(true),
		m_oglusenpot(true),
		m_oglMipmapping(false),
		m_oglMonochrome(false),
		m_oglTextureFilter(TEXTURE_FILTER_NONE),
		m_oglDepthBuffer(false),
		m_alphaTestValue(0.3),
		m_screenwidth(800),
		m_screenheight(600),
		m_windowtitle("FIFE"),
		m_windowicon(""),
		m_defaultfontpath("fonts/FreeSans.ttf"),
		m_defaultfontsize(8),
		m_defaultfontglyphs("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&amp;`'*#=[]\\\""),
		m_iscolorkeyenabled(false),
		m_lighting(0),
		m_isframelimit(false),
		m_framelimit(60),
		m_mousesensitivity(0.0),
		m_mouseacceleration(false) {
			m_colorkey.r = 255;
			m_colorkey.g = 0;
			m_colorkey.b = 255;

#if defined( __unix__ )
			m_videodriver = "x11";
#elif defined( WIN32 )
			m_videodriver = "windib";
#elif defined( __APPLE_CC__ )
			m_videodriver = "x11";
#else
			m_videodriver = "";
#endif

	}

	EngineSettings::~EngineSettings() {
	}

	void EngineSettings::setBitsPerPixel(uint8_t bitsperpixel) {
		std::vector<uint8_t> pv = getPossibleBitsPerPixel();
		std::vector<uint8_t>::iterator i = std::find(pv.begin(), pv.end(), bitsperpixel);
		if (i != pv.end()) {
			m_bitsperpixel = bitsperpixel;
			return;
		}

		FL_WARN(_log, LMsg("EngineSettings::setBitsPerPixel() - ")
			<< " Tried to set screen bpp to an unsupporded value of " << bitsperpixel <<
			".  Setting bpp to use the default value of 0 (the current screen bpp)");

		m_bitsperpixel = 0;  //default value
	}

	std::vector<uint8_t> EngineSettings::getPossibleBitsPerPixel() const {
		std::vector<uint8_t> tmp;
		tmp.push_back(0);
		tmp.push_back(16);
		tmp.push_back(24);
		tmp.push_back(32);
		return tmp;
	}

	void EngineSettings::setInitialVolume(float volume) {
		if (volume > getMaxVolume() || volume < 0) {
			FL_WARN(_log, LMsg("EngineSettings::setInitialVolume() - ")
				<< " Tried to set initial volume to an unsupporded value of " << volume <<
				".  Setting volume to the default value of 5 (minumum is 0, maximum is 10)");

			m_initialvolume = 5.0;
			return;
		}

		m_initialvolume = volume;
	}

	float EngineSettings::getMaxVolume() const {
		return MAXIMUM_VOLUME;
	}

	void EngineSettings::setRenderBackend(const std::string& renderbackend) {
		std::vector<std::string> pv = getPossibleRenderBackends();
		std::vector<std::string>::iterator i = std::find(pv.begin(), pv.end(), renderbackend);
		if (i != pv.end()) {
			m_renderbackend = renderbackend;
			return;
		}
		FL_WARN(_log, LMsg("EngineSettings::setRenderBackend() - ")
			<< renderbackend << " is not a valid render backend " <<
			".  Setting the render backend to the default value of \"SDL\".");

		m_renderbackend = "SDL";
	}

	std::vector<std::string> EngineSettings::getPossibleRenderBackends() {
		std::vector<std::string> tmp;
		tmp.push_back("SDL");
		tmp.push_back("OpenGL");
		return tmp;
	}

	void EngineSettings::setSDLRemoveFakeAlpha(bool sdlremovefakealpha) {
		m_sdlremovefakealpha = sdlremovefakealpha;
	}

	void EngineSettings::setGLCompressImages(bool oglcompressimages) {
		m_oglcompressimages = oglcompressimages;
	}

	void EngineSettings::setGLUseFramebuffer(bool ogluseframebuffer) {
		m_ogluseframebuffer = ogluseframebuffer;
	}

	void EngineSettings::setGLUseNPOT(bool oglusenpot) {
		m_oglusenpot = oglusenpot;
	}

	void EngineSettings::setGLTextureFiltering(TextureFiltering filter) {
		m_oglTextureFilter = filter;
	}

	TextureFiltering EngineSettings::getGLTextureFiltering() const {
		return m_oglTextureFilter;
	}

	void EngineSettings::setGLUseMipmapping(bool mipmapping) {
		m_oglMipmapping = mipmapping;
	}

	bool EngineSettings::isGLUseMipmapping() const {
		return m_oglMipmapping;
	}

	void EngineSettings::setGLUseMonochrome(bool monochrome) {
		m_oglMonochrome = monochrome;
	}

	bool EngineSettings::isGLUseMonochrome() const {
		return m_oglMonochrome;
	}

	void EngineSettings::setGLUseDepthBuffer(bool buffer) {
		m_oglDepthBuffer = buffer;
	}

	bool EngineSettings::isGLUseDepthBuffer() const {
		return m_oglDepthBuffer;
	}

	void EngineSettings::setGLAlphaTestValue(float alpha) {
		m_alphaTestValue = alpha;
	}

	float EngineSettings::getGLAlphaTestValue() const {
		return m_alphaTestValue;
	}

	void EngineSettings::setScreenWidth(uint16_t screenwidth) {
		m_screenwidth = screenwidth;
	}

	void EngineSettings::setScreenHeight(uint16_t screenheight) {
		m_screenheight = screenheight;
	}

	void EngineSettings::setDefaultFontPath(const std::string& defaultfontpath) {
		m_defaultfontpath = defaultfontpath;
	}

	void EngineSettings::setDefaultFontSize(uint16_t defaultfontsize) {
		m_defaultfontsize = defaultfontsize;
	}

	void EngineSettings::setDefaultFontGlyphs(const std::string& defaultfontglyphs) {
		m_defaultfontglyphs = defaultfontglyphs;
	}

	void EngineSettings::setWindowTitle(const std::string& title) {
		m_windowtitle = title;
	}

	void EngineSettings::setWindowIcon(const std::string& icon) {
		m_windowicon = icon;
	}

	void EngineSettings::setColorKeyEnabled(bool colorkeyenable) {
		m_iscolorkeyenabled = colorkeyenable;
	}

	bool EngineSettings::isColorKeyEnabled() const {
		return m_iscolorkeyenabled;
	}

	void EngineSettings::setColorKey(uint8_t r, uint8_t g, uint8_t b) {
		m_colorkey.r = r;
		m_colorkey.g = g;
		m_colorkey.b = b;
	}

	const SDL_Color& EngineSettings::getColorKey() const {
		return m_colorkey;
	}

	void EngineSettings::setVideoDriver(const std::string& driver) {
		//TODO: validate the video driver
		m_videodriver = driver;
	}

	const std::string& EngineSettings::getVideoDriver() const {
		return m_videodriver;
	}
	void EngineSettings::setLightingModel(uint32_t lighting) {
		if (lighting <= 2) {
			m_lighting = lighting;
			return;
		}

		FL_WARN(_log, LMsg("EngineSettings::setLightingModel() - ")
			<< lighting << " is not a valid lighting model." <<
			".  Setting the lighting model to the default value of 0 (off)");

		m_lighting = 0;
	}

	void EngineSettings::setFrameLimitEnabled(bool limited) {
		m_isframelimit = limited;
	}

	bool EngineSettings::isFrameLimitEnabled() const {
		return m_isframelimit;
	}

	void EngineSettings::setFrameLimit(uint16_t framelimit) {
		m_framelimit = framelimit;
	}

	uint16_t EngineSettings::getFrameLimit() const {
		return m_framelimit;
	}

	void EngineSettings::setMouseSensitivity(float sens) {
		m_mousesensitivity = sens;
	}

	float EngineSettings::getMouseSensitivity() const {
		return m_mousesensitivity;
	}

	void EngineSettings::setMouseAccelerationEnabled(bool acceleration) {
		m_mouseacceleration = acceleration;
	}

	bool EngineSettings::isMouseAccelerationEnabled() const {
		return m_mouseacceleration;
	}
}

