/***************************************************************************
 *   Copyright (C) 2005-2010 by the FIFE team                              *
 *   http://www.fifengine.net                                               *
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

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "devicecaps.h"

namespace FIFE {

	ScreenMode::ScreenMode() :
		           m_width(0), m_height(0), m_bpp(0), m_SDLFlags(0){
	}

	ScreenMode::ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags) :
		           m_width(width), m_height(height), m_bpp(bpp), m_SDLFlags(SDLFlags){
	}

	ScreenMode::ScreenMode(const ScreenMode& rhs){
		m_width = rhs.getWidth();
		m_height = rhs.getHeight();
		m_bpp = rhs.getBPP();
		m_SDLFlags = rhs.getSDLFlags();
	}

	DeviceCaps::DeviceCaps() {
	}


	DeviceCaps::~DeviceCaps() {
	}

	void DeviceCaps::fillDeviceCaps() {
		int numBPP = 1;
		int bpps[numBPP];

		int numResolutions = 15;

		//FLAGS
#ifdef HAVE_OPENGL
		int numFlags = 4;
		Uint32 flags[numFlags];

		//OpenGL, windowed, hw accel
		flags[0] = SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL;
		//OpenGL, fullscree, hw accel
		flags[1] = SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL | SDL_FULLSCREEN;
		//SDL, windowed
		flags[2] = 0;
		//SDL, fullscreen
		flags[3] = SDL_FULLSCREEN;
#else
		int numFlags = 2;
		Uint32 flags[numFlags];

		//SDL, windowed
		flags[0] = 0;
		//SDL, fullscreen
		flags[1] = SDL_FULLSCREEN;
#endif
		//BITS PER PIXEL

		bpps[0] = 32;


		//RESOLUTIONS
		int resolutions[15][2] = {
			{640, 480},
			{800, 600},
			{1024, 768},
			{1152, 864},
			{1280, 768},
			{1280, 800},
			{1280, 960},
			{1280, 1024},
			{1366, 768},
			{1440, 900},
			{1600, 900},
			{1600, 1200},
			{1680, 1050},
			{1920, 1080},
			{1920, 1200}
		};

		int bpp;

		for (int i = 0; i < numBPP; i++){
			for (int j = 0; j < numFlags; j++) {
				for (int k = 0; k < numResolutions; k++) {
					bpp = SDL_VideoModeOK(resolutions[k][0], resolutions[k][1], bpps[i], flags[j]);
					if (bpp > 0) {
//						std::cout << resolutions[k][0] << "x" << resolutions[k][1] << ":" << bpp << std::endl;
						ScreenMode mode = ScreenMode(resolutions[k][0], resolutions[k][1], bpps[i], flags[j]);
						m_screenModes.push_back(mode);
					}
				}
			}
		}

	}

} //FIFE
