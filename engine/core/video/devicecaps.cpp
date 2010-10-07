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

	DeviceCaps::DeviceCaps() :
	    m_driverName("Invalid"),
		m_hwAvailable(false),
		m_wmAvailable(false),
		m_hwBlitAccel(false),
		m_hwCCBlitAccel(false),
		m_hwToHwAlphaBlitAccel(false),
		m_swToHwBlitAccel(false),
		m_swToHwCCBlistAccel(false),
		m_swToHwAlphaBlitAccel(false),
		m_BlitFillAccel(false),
		m_videoMem(0) {

		fillAvailableDrivers();
	}


	DeviceCaps::~DeviceCaps() {
	}

	void DeviceCaps::reset() {
		m_screenModes.clear();
		m_driverName = "Invalid";
		m_hwAvailable = false;
		m_wmAvailable = false;
		m_hwBlitAccel = false;
		m_hwCCBlitAccel = false;
		m_hwToHwAlphaBlitAccel = false;
		m_swToHwBlitAccel = false;
		m_swToHwCCBlistAccel = false;
		m_swToHwAlphaBlitAccel = false;
		m_BlitFillAccel = false;
		m_videoMem = 0;
	}


	void DeviceCaps::fillAvailableDrivers() {
		m_availableDrivers.clear();
#if defined( __unix__ )
		m_availableDrivers.push_back("x11");
		m_availableDrivers.push_back("nanox");
		m_availableDrivers.push_back("qtopia");
		m_availableDrivers.push_back("fbcon");
		m_availableDrivers.push_back("directfb");
		m_availableDrivers.push_back("svgalib");
#endif

// Win32
#if defined( WIN32 )
		m_availableDrivers.push_back("directx");
		m_availableDrivers.push_back("windib");
#endif

// Macintosh
#if defined( __APPLE_CC__ )
		m_availableDrivers.push_back("Quartz");
		m_availableDrivers.push_back("x11");
#endif
	}

	void DeviceCaps::fillDeviceCaps() {
		int bufferSize = 256;
		char buffer[bufferSize];

		int numBPP = 1;
		int bpps[numBPP];

		SDL_Rect **modes;

		//clear in case this is called twice
		reset();

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

		for (int i = 0; i < numBPP; ++i){
			for (int j = 0; j < numFlags; ++j) {
				modes = SDL_ListModes(NULL, flags[j]);

				if (modes == (SDL_Rect**)0) {
					//no modes found
					break;
				}

				if (modes == (SDL_Rect**)-1) {
					//All screen modes are available with the specified flags (a windowed mode most likely)
					ScreenMode mode = ScreenMode(0, 0, bpps[i], flags[j]);
					m_screenModes.push_back(mode);
					continue;
				}

				for (int k = 0; modes[k]; ++k) {
					int bpp = SDL_VideoModeOK(modes[k]->w, modes[k]->h, bpps[i], flags[j]);
					if (bpp > 0) {
						ScreenMode mode = ScreenMode(modes[k]->w, modes[k]->h, bpps[i], flags[j]);
						m_screenModes.push_back(mode);
					}
				}
				modes = (SDL_Rect**)0;
			}
		}

		if(SDL_VideoDriverName(buffer, bufferSize) != NULL) {
			m_driverName = std::string(buffer);
		}
		else {
			m_driverName = "Unknown";
		}

		const SDL_VideoInfo* vInfo = SDL_GetVideoInfo();

		m_hwAvailable = vInfo->hw_available;
		m_wmAvailable = vInfo->wm_available;
		m_hwBlitAccel = vInfo->blit_hw;
		m_hwCCBlitAccel = vInfo->blit_hw_CC;
		m_hwToHwAlphaBlitAccel = vInfo->blit_hw_A;
		m_swToHwBlitAccel = vInfo->blit_sw;
		m_swToHwCCBlistAccel = vInfo->blit_sw_CC;
		m_swToHwAlphaBlitAccel = vInfo->blit_sw_A;
		m_BlitFillAccel = vInfo->blit_fill;
		m_videoMem = vInfo->video_mem;
	}

} //FIFE
