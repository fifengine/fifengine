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

#ifndef FIFE_DEVICECAPS_H
#define FIFE_DEVICECAPS_H

// Standard C++ library includes
#include <string>
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class ScreenMode {
	public:
		/** Default Constructor
		 * @note You shouldn't construct these objects yourself.  This default
		 * constructor was provided because swig complained that was none.
		 */
		ScreenMode();
		ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags);
		ScreenMode(const ScreenMode& rhs);

		/** Destructor.
		 */
		~ScreenMode() {};

		bool operator <(const ScreenMode& rhs) const;

		/** Returns the width of the screen mode.
		 * @note If both width and height are 0 it means that ALL modes are available
		 * for use with the specified flags.  Most likely this is a windowed mode.
		 */
		uint16_t getWidth() const { return m_width; };

		/** Returns the height of the screen mode.
		 * @note If both width and height are 0 it means that ALL modes are available
		 * for use with the specified flags.  Most likely this is a windowed mode.
		 */
		uint16_t getHeight() const { return m_height; };

		/** Returns the number of bits per pixel this mode uses.
		 */
		uint16_t getBPP() const { return m_bpp; };

		/** Returns the SDL flags used when testing this mode.
		 */
		uint32_t getSDLFlags() const { return m_SDLFlags; };

		/** True if this is a fullscreen mode.  False if it is a windowed mode.
		 */
		bool isFullScreen() const { return (m_SDLFlags & SDL_FULLSCREEN) ? true : false;};

		/** True if this mode uses the OpenGL renderer.  False otherwise.
		 */
		bool isOpenGL() const { return (m_SDLFlags & SDL_OPENGL) ? true : false; };

		/** Is this screen mode an SDL only screen mode
		 */
		bool isSDL() const { return (!(m_SDLFlags & SDL_OPENGL)) ? true : false; };

		/** Returns true if this is a SDL screen mode with the SDL hardware surface enabled
		 */
		bool isSDLHardwareSurface() const { return (m_SDLFlags & SDL_HWSURFACE) ? true : false; };


		//OpenGL, windowed, hw accel
		static const uint32_t HW_WINDOWED_OPENGL = SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL;
		//OpenGL, fullscreen, hw accel
		static const uint32_t HW_FULLSCREEN_OPENGL = SDL_OPENGL | SDL_HWPALETTE | SDL_HWACCEL | SDL_FULLSCREEN;
		//SDL, windowed
		static const uint32_t WINDOWED_SDL = 0;
		//SDL, windowed, HW surface and double buffer
		static const uint32_t WINDOWED_SDL_DB_HW = SDL_HWSURFACE | SDL_DOUBLEBUF;
		//SDL, fullscreen
		static const uint32_t FULLSCREEN_SDL = SDL_FULLSCREEN;
		//SDL, fullscreen, HW surface and double buffer
		static const uint32_t FULLSCREEN_SDL_DB_HW = SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF;

	private:
		uint16_t m_width;
		uint16_t m_height;
		uint16_t m_bpp;
		uint32_t m_SDLFlags;

	};  //ScreenMode

	class DeviceCaps {
	public:
		/** Constructor.
		 */
		DeviceCaps();

		/** Destructor.
		 */
		~DeviceCaps();

		/** Should be called AFTER SDL_Init() has been called
		 */
		void fillDeviceCaps();

		/** Clears all information gathered for the device
		 */
		void reset();

		/** Gets the available graphics drivers for your operating system
		 */
		std::vector<std::string> getAvailableDrivers() const { return m_availableDrivers; };

		/** Returns a vector containing screen modes.
		 */
		std::vector<ScreenMode> getSupportedScreenModes() const { return m_screenModes; };

		/** Gets the nearest valid screen mode based on the arguments passed
		 */
		ScreenMode getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const;

		/** Returns the name of the current video driver.
		 */
		std::string getDriverName() const { return m_driverName; };

		/** Is it possible to create hardware surfaces ?
		 */
		bool isHwSurfaceAvail() const { return m_hwAvailable; };

		/** Is there a window manager available ?
		 */
		bool isWindowManagerAvail() const { return m_wmAvailable;} ;

		/** Are hardware to hardware blits accelerated ?
		 */
		bool isHwBlitAccel() const { return m_hwBlitAccel; };

		/** Are hardware to hardware colorkey blits accelerated ?
		 */
		bool isHwColorkeyBlitAccel() const { return m_hwCCBlitAccel; };

		/** Are hardware to hardware alpha blits accelerated ?
		 */
		bool isHwAlphaBlitAccel() const { return m_hwToHwAlphaBlitAccel; };

		/** Are software to hardware blits accelerated ?
		 */
		bool isSwToHwBlitAccel() const { return m_swToHwBlitAccel; };

		/** Are software to hardware colorkey blits accelerated ?
		 */
		bool isSwToHwColorkeyBlitAccel() const { return m_swToHwCCBlistAccel; };

		/** Are software to hardware alpha blits accelerated ?
		 */
		bool isSwToHwAlphaBlitAccel() const { return m_swToHwAlphaBlitAccel; };

		/** Are color fills accelerated ?
		 */
		bool isBlitFillAccel() const { return m_BlitFillAccel; };

		/** Total amount of video memory in Kilobytes, only valid if hardware sufaces are available.
		 */
		uint32_t getVideoMemory() const { return m_videoMem; };

	private:
		std::vector<ScreenMode> m_screenModes;
		std::string m_driverName;
		std::vector<std::string> m_availableDrivers;

		bool m_hwAvailable;
		bool m_wmAvailable;
		bool m_hwBlitAccel;
		bool m_hwCCBlitAccel;
		bool m_hwToHwAlphaBlitAccel;
		bool m_swToHwBlitAccel;
		bool m_swToHwCCBlistAccel;
		bool m_swToHwAlphaBlitAccel;
		bool m_BlitFillAccel;

		uint32_t m_videoMem;

		/** Called in the constructor.  No need for anyone to call this
		 */
		void fillAvailableDrivers();
	}; //DeviceCaps
} //FIFE



#endif //FIFE_DEVICECAPS_H
