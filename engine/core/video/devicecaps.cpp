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
#include <iostream>
#include <algorithm>

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "devicecaps.h"

namespace FIFE {

	ScreenMode::ScreenMode() :
		           m_width(0), m_height(0), m_bpp(0), m_refreshRate(0), m_SDLFlags(0),
				m_format(0), m_display(0), m_renderDriver(""), m_renderDriverIndex(-1) {
	}

	ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags) :
		           m_width(width), m_height(height), m_bpp(bpp), m_refreshRate(0), m_SDLFlags(SDLFlags),
				m_format(0), m_display(0), m_renderDriver(""), m_renderDriverIndex(-1){
	}

	ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint16_t rate, uint32_t SDLFlags) :
		           m_width(width), m_height(height), m_bpp(bpp), m_refreshRate(rate), m_SDLFlags(SDLFlags),
				m_format(0), m_display(0), m_renderDriver(""), m_renderDriverIndex(-1){
	}

	ScreenMode::ScreenMode(const ScreenMode& rhs){
		m_width = rhs.getWidth();
		m_height = rhs.getHeight();
		m_bpp = rhs.getBPP();
		m_SDLFlags = rhs.getSDLFlags();
		m_refreshRate = rhs.getRefreshRate();
		m_format = rhs.getFormat();
		m_display = rhs.getDisplay();
		m_renderDriver = rhs.getRenderDriverName();
		m_renderDriverIndex = rhs.getRenderDriverIndex();
	}

	bool ScreenMode::operator <(const ScreenMode& rhs) const {
		// first by display, from lower to higher
		if (m_display > rhs.getDisplay()){
			return true;
		}
		else if (m_display < rhs.getDisplay()){
			return false;
		}

		//sort by fullscreen first
		if (!isFullScreen() && rhs.isFullScreen()){
			return true;
		}
		else if (isFullScreen() && !rhs.isFullScreen()){
			return false;
		}

		//next by bpp
		if (m_bpp < rhs.getBPP()){
			return true;
		}
		else if (m_bpp > rhs.getBPP()){
			return false;
		}

		//then by screen dimensions
		if (m_width < rhs.getWidth() || m_height < rhs.getHeight()) {
			return true;
		} else if (m_width > rhs.getWidth() || m_height > rhs.getHeight()) {
			return false;
		}
		//last by refresh rate
		if (m_refreshRate < rhs.getRefreshRate()) {
			return true;
		}

		return false;
	}

	DeviceCaps::DeviceCaps() :
	    m_videoDriverName("dummy"),
		m_renderDriverName(""),
		m_renderDriverIndex(-1) {
	}

	DeviceCaps::~DeviceCaps() {
	}

	void DeviceCaps::reset() {
		m_screenModes.clear();
		m_renderDriverName = "";
		m_renderDriverIndex = -1;

		fillAvailableDrivers();
	}

	void DeviceCaps::fillAvailableDrivers() {
		// video driver section (x11, windows, dummy, ...)
		m_availableVideoDrivers.clear();
		uint8_t driverCount = SDL_GetNumVideoDrivers();
		for (uint8_t i = 0; i != driverCount; i++) {
			std::string driver(SDL_GetVideoDriver(i));
			m_availableVideoDrivers.push_back(driver);
		}
		m_videoDriverName = std::string(SDL_GetCurrentVideoDriver());

		// render driver section (opengl, direct3d, software, ...)
		m_availableRenderDrivers.clear();
		SDL_RendererInfo info;
		driverCount = SDL_GetNumRenderDrivers();
		for (uint8_t i = 0; i != driverCount; i++) {
			SDL_GetRenderDriverInfo(i, &info);
			std::string name(info.name);
			m_availableRenderDrivers.push_back(name);
		}
	}

	void DeviceCaps::fillDeviceCaps() {
		//clear in case this is called twice
		m_screenModes.clear();
		fillAvailableDrivers();
		//FLAGS
#ifdef HAVE_OPENGL
		const uint32_t numFlags = 4;
		uint32_t flags[numFlags];

		//OpenGL, windowed
		flags[0] = ScreenMode::WINDOWED_OPENGL;
		//OpenGL, fullscreen
		flags[1] = ScreenMode::FULLSCREEN_OPENGL;
		//SDL, windowed
		flags[2] = ScreenMode::WINDOWED_SDL;
		//SDL, fullscreen
		flags[3] = ScreenMode::FULLSCREEN_SDL;

#else
		const uint32_t numFlags = 2;
		uint32_t flags[numFlags];

		//SDL, windowed
		flags[0] = ScreenMode::WINDOWED_SDL;
		//SDL, fullscreen
		flags[1] = ScreenMode::FULLSCREEN_SDL;
#endif

		//BITS PER PIXEL
		const uint32_t numBPP = 3;
		uint16_t bpps[numBPP];

		bpps[0] = 16;
		bpps[1] = 24;
		bpps[2] = 32;

		bool renderDriver = m_renderDriverIndex != -1;
		uint8_t displayCount = SDL_GetNumVideoDisplays();
		for (uint8_t i = 0; i != displayCount; i++) {
			SDL_DisplayMode mode;
			uint8_t displayModes = SDL_GetNumDisplayModes(i);
			for (uint8_t m = 0; m != displayModes; m++) {
				if (SDL_GetDisplayMode(i, m, &mode) == 0) {
					for (uint32_t ii = 0; ii < numBPP; ++ii){
						for (uint32_t j = 0; j < numFlags; ++j) {
							//m_screenModes.push_back(ScreenMode(mode.w, mode.h, SDL_BITSPERPIXEL(mode.format), mode.refresh_rate, flags[j]));
							ScreenMode m(mode.w, mode.h, bpps[ii], mode.refresh_rate, flags[j]);
							m.setFormat(mode.format);
							m.setDisplay(i);
							if (renderDriver) {
								m.setRenderDriverName(m_renderDriverName);
								m.setRenderDriverIndex(m_renderDriverIndex);
							}
							m_screenModes.push_back(m);
						}
					}
				} else {
					throw SDLException(SDL_GetError());
				}
			}
		}

		//sort the list to keep the most preferred modes at the top of the selection process
		std::sort(m_screenModes.begin(), m_screenModes.end());
		std::reverse(m_screenModes.begin(), m_screenModes.end());
	}

	ScreenMode DeviceCaps::getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const {
		// refresh rate is set to 0 so that desktop setting is used and the first display is used
		return getNearestScreenMode(width, height, bpp, renderer, fs, 0, 0);
	}

	ScreenMode DeviceCaps::getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs, uint16_t refresh, uint8_t display) const {
		ScreenMode mode;
		SDL_DisplayMode target, closest;
		bool foundMode = false;

		// Set the desired resolution, etc.
		target.w = width;
		target.h = height;
		if (bpp == 0) {
			target.format = 0;  // don't care, should be desktop bpp
		} else if (bpp == 16) {
			target.format = SDL_PIXELFORMAT_RGB565;
		} else {
			target.format = SDL_PIXELFORMAT_RGB888;
		}
		target.refresh_rate = refresh;
		target.driverdata   = 0; // initialize to 0

		// only first display
		if (SDL_GetClosestDisplayMode(display, &target, &closest)) {
			uint32_t flags = 0;
			if (renderer == "OpenGL") {
				if (fs) {
					flags =	SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
				} else {
					flags =	SDL_WINDOW_OPENGL;
				}
			} else {
				if (fs) {
					flags =	SDL_WINDOW_FULLSCREEN;
				}
			}
			mode = ScreenMode(closest.w, closest.h, bpp, closest.refresh_rate, flags);
			mode.setFormat(closest.format);
			mode.setDisplay(display);
			if (m_renderDriverIndex != -1) {
				mode.setRenderDriverName(m_renderDriverName);
				mode.setRenderDriverIndex(m_renderDriverIndex);
			}
			foundMode = true;
		}

		if (!foundMode) {
			throw NotSupported("Could not find a matching screen mode for the values given!");
		}

		return mode;
	}

	void DeviceCaps::setRenderDriverName(const std::string& driver) {
		bool found = false;
		uint8_t driverCount = m_availableRenderDrivers.size();
		for (uint8_t i = 0; i != driverCount; i++) {
			if (driver == m_availableRenderDrivers[i]) {
				m_renderDriverName = driver;
				m_renderDriverIndex = i;
				found = true;
				break;
			}
		}
		if (!found) {
			if (driver != "") {
				throw NotSupported("Could not find a matching render driver!");
			}
			m_renderDriverName = "";
			m_renderDriverIndex = -1;
		}
		// refill
		fillDeviceCaps();
	}

	uint8_t DeviceCaps::getDisplayCount() const {
		uint8_t displayCount = SDL_GetNumVideoDisplays();
		return displayCount;
	}

	std::string DeviceCaps::getDisplayName(uint8_t display) const {
		if (display >= getDisplayCount()) {
			throw NotSupported("Could not find a matching display!");
			return std::string("Invalid");
		}
		std::string displayName(SDL_GetDisplayName(display));
		return displayName;
	}

	uint32_t DeviceCaps::getDesktopFormat(uint8_t display) const {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(display, &mode) != 0) {
			throw SDLException(SDL_GetError());
		}
		return mode.format;
	}

	int32_t DeviceCaps::getDesktopRefreshRate(uint8_t display) const {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(display, &mode) != 0) {
			throw SDLException(SDL_GetError());
		}
		return mode.refresh_rate;
	}

	int32_t DeviceCaps::getDesktopWidth(uint8_t display) const {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(display, &mode) != 0) {
			throw SDLException(SDL_GetError());
		}
		return mode.w;
	}

	int32_t DeviceCaps::getDesktopHeight(uint8_t display) const {
		SDL_DisplayMode mode;
		if (SDL_GetDesktopDisplayMode(display, &mode) != 0) {
			throw SDLException(SDL_GetError());
		}
		return mode.h;
	}

	Rect DeviceCaps::getDisplayBounds(uint8_t display) const {
		SDL_Rect srec;
		if (SDL_GetDisplayBounds(display, &srec) != 0) {
			throw SDLException(SDL_GetError());
		}
		Rect rec(srec.x, srec.y, srec.w, srec.h);
		return rec;
	}

} //FIFE
