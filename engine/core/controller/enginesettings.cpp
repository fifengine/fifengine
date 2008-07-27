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
		m_sldremovefakealpha(false),
		m_screenwidth(800),
		m_screenheight(600),
		m_windowtitle("FIFE"),
		m_windowicon(""),
		m_defaultfontpath(""),
		m_defaultfontsize(8),
		m_defaultfontglyphs(""),
		m_image_chunking_size(256) {
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

	std::vector<std::pair<unsigned int, unsigned int> > EngineSettings::getPossibleResolutions() const {
		SDL_Rect **modes = SDL_ListModes(NULL, ((getRenderBackend() != "SDL") ? (SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL) : 0) | (isFullScreen() ? SDL_FULLSCREEN : 0));
		if(modes == (SDL_Rect **)0)
			throw NotFound("No VideoMode Found");

		std::vector<std::pair<unsigned int, unsigned int> > result;
		if(modes != (SDL_Rect **)-1)
			for(unsigned int i = 0; modes[i]; ++i)
				result.push_back(std::pair<unsigned int, unsigned int>(modes[i]->w, modes[i]->h));
		return result;
	}

	void EngineSettings::setBitsPerPixel(unsigned int bitsperpixel) {
		std::vector<unsigned int> pv = getPossibleBitsPerPixel();
		std::vector<unsigned int>::iterator i = std::find(pv.begin(), pv.end(), bitsperpixel);
		if (i != pv.end()) {
			m_bitsperpixel = bitsperpixel;
			return;
		}
		throw NotSupported("Given bits per pixel value is not supported");
	}
	
	std::vector<unsigned int> EngineSettings::getPossibleBitsPerPixel() const {
		std::vector<unsigned int> tmp;
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
	
	void EngineSettings::setSDLRemoveFakeAlpha(bool sldremovefakealpha) {
		m_sldremovefakealpha = sldremovefakealpha;
	}
	
	void EngineSettings::setScreenWidth(unsigned int screenwidth) {
		m_screenwidth = screenwidth;
	}
	
	void EngineSettings::setScreenHeight(unsigned int screenheight) {
		m_screenheight = screenheight;
	}
	
	void EngineSettings::setDefaultFontPath(const std::string& defaultfontpath) {
		m_defaultfontpath = defaultfontpath;
	}
	
	void EngineSettings::setDefaultFontSize(const unsigned int defaultfontsize) {
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
	
}

