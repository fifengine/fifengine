/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
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

#include "enginesettings.h"

namespace FIFE {
	const float MAXIMUM_VOLUME = 10.0;

	EngineSettings::EngineSettings():
		m_bitsperpixel(0),
		m_fullscreen(false),
		m_initialvolume(MAXIMUM_VOLUME / 2),
		m_renderbackend("SDL"),
		m_sdlremovefakealpha(false),
		m_screenwidth(800),
		m_screenheight(600),
		m_windowtitle("FIFE"),
		m_windowicon(""),
		m_defaultfontpath(""),
		m_defaultfontsize(8),
		m_defaultfontglyphs(""),
		m_iscolorkeyenabled(false),
		m_lighting(0) {
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

	void EngineSettings::validate() const {
		if (m_defaultfontpath == "") {
			throw NotSet("Path for default font is not set");
		}
		std::string::size_type loc = m_defaultfontpath.find(".ttf", 0);
		if ((loc == std::string::npos) && (m_defaultfontglyphs == "")) {
			throw NotSet("Glyphs for default font are not set");
		}
	}

	std::vector<std::pair<uint16_t, uint16_t> > EngineSettings::getPossibleResolutions() const {
		SDL_Rect **modes = SDL_ListModes(NULL, ((getRenderBackend() != "SDL") ? (SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL) : 0) | (isFullScreen() ? SDL_FULLSCREEN : 0));
		if(modes == (SDL_Rect **)0)
			throw NotFound("No VideoMode Found");

		std::vector<std::pair<uint16_t, uint16_t> > result;
		if(modes != (SDL_Rect **)-1)
			for(unsigned int i = 0; modes[i]; ++i)
				result.push_back(std::pair<uint16_t, uint16_t>(modes[i]->w, modes[i]->h));
		return result;
	}

	void EngineSettings::setBitsPerPixel(uint16_t bitsperpixel) {
		std::vector<uint16_t> pv = getPossibleBitsPerPixel();
		std::vector<uint16_t>::iterator i = std::find(pv.begin(), pv.end(), bitsperpixel);
		if (i != pv.end()) {
			m_bitsperpixel = bitsperpixel;
			return;
		}
		throw NotSupported("Given bits per pixel value is not supported");
	}

	std::vector<uint16_t> EngineSettings::getPossibleBitsPerPixel() const {
		std::vector<uint16_t> tmp;
		tmp.push_back(0);
		tmp.push_back(16);
		tmp.push_back(24);
		tmp.push_back(32);
		return tmp;
	}

	void EngineSettings::setInitialVolume(float volume) {
		if (volume > getMaxVolume()) {
			throw NotSupported("Given volume exceeds maximum volume");
		}
		if (volume < 0) {
			throw NotSupported("Given volume is below 0");
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
		throw NotSupported("Given render backend is not supported");
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
		m_videodriver = driver;
	}

	const std::string& EngineSettings::getVideoDriver() const {
		return m_videodriver;
	}
	void EngineSettings::setLightingModel(unsigned int lighting) {
		if (lighting <= 2) {
			m_lighting = lighting;
			return;
		}
		throw NotSupported("Given light model is not supported");
	}

}

