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
#include "util/structures/rect.h"

namespace FIFE {

	class ScreenMode {
	public:
		/** Default Constructor
		 * @note You shouldn't construct these objects yourself.  This default
		 * constructor was provided because swig complained that was none.
		 */
		ScreenMode();
		ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags);
		ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint16_t rate, uint32_t SDLFlags);
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

		/** Returns the refresh rate in Hz of this mode.
		 */
		uint16_t getRefreshRate() const { return m_refreshRate; }

		/** Returns the SDL flags used when testing this mode.
		 */
		uint32_t getSDLFlags() const { return m_SDLFlags; };

		/** True if this is a fullscreen mode.  False if it is a windowed mode.
		 */
		bool isFullScreen() const { return (m_SDLFlags & SDL_WINDOW_FULLSCREEN) ? true : false;};

		/** True if this mode uses the OpenGL renderer.  False otherwise.
		 */
		bool isOpenGL() const { return (m_SDLFlags & SDL_WINDOW_OPENGL) ? true : false; };

		/** Is this screen mode an SDL only screen mode
		 */
		bool isSDL() const { return (!(m_SDLFlags & SDL_WINDOW_OPENGL)) ? true : false; };

		/** Sets the pixel format enum.
		 */
		void setFormat(uint32_t format) { m_format = format;}

		/** Returns the pixel format enum.
		 */
		uint32_t getFormat() const { return m_format; }

		/** Sets the display index.
		 */
		void setDisplay(uint8_t display) { m_display = display; }

		/** Returns the display index.
		 */
		uint8_t getDisplay() const { return m_display; }
		
		/** Sets the render driver name.
		 */
		void setRenderDriverName(const std::string driver) { m_renderDriver = driver; }

		/** Returns the render driver name. Default is "".
		 */
		const std::string& getRenderDriverName() const { return m_renderDriver; }

		/** Sets the index of the render driver used by SDL.
		 */
		void setRenderDriverIndex(int8_t index) { m_renderDriverIndex = index; }

		/** Returns the index of the render driver. Default is -1.
		 */
		int8_t getRenderDriverIndex() const { return m_renderDriverIndex; }

		//OpenGL, windowed
		static const uint32_t WINDOWED_OPENGL = SDL_WINDOW_OPENGL;
		//OpenGL, fullscreen
		static const uint32_t FULLSCREEN_OPENGL = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
		//SDL, windowed
		static const uint32_t WINDOWED_SDL = 0;
		//SDL, fullscreen
		static const uint32_t FULLSCREEN_SDL = SDL_WINDOW_FULLSCREEN;

	private:
		uint16_t m_width;
		uint16_t m_height;
		uint16_t m_bpp;
		uint16_t m_refreshRate;
		uint32_t m_SDLFlags;
		uint32_t m_format;
		uint8_t m_display;
		std::string m_renderDriver;
		int8_t m_renderDriverIndex;
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

		/** Gets the available video drivers for your operating system
		 */
		std::vector<std::string> getAvailableVideoDrivers() const { return m_availableVideoDrivers; };

		/** Gets the available render drivers for your operating system
		 */
		std::vector<std::string> getAvailableRenderDrivers() const { return m_availableRenderDrivers; };

		/** Returns a vector containing screen modes.
		 */
		std::vector<ScreenMode> getSupportedScreenModes() const { return m_screenModes; };

		/** Gets the nearest valid screen mode based on the arguments passed
		 */
		ScreenMode getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs) const;

		/** Gets the nearest valid screen mode based on the arguments passed
		 */
		ScreenMode getNearestScreenMode(uint16_t width, uint16_t height, uint16_t bpp, const std::string& renderer, bool fs, uint16_t refresh, uint8_t display = 0) const;

		/** Returns the name of the current video driver.
		 */
		std::string getVideoDriverName() const { return m_videoDriverName; }

		/** Sets the name of the video driver.
		 */
		void setVideoDriverName(const std::string& driver) { m_videoDriverName = driver; }

		/** Returns the name of the current render driver or
		 *  an empty string to initialize the first one supporting the requested flags.
		 */
		std::string getRenderDriverName() const { return m_renderDriverName; }

		/** Sets the name of the render driver.
		 */
		void setRenderDriverName(const std::string& driver);

		/** Returns the number of displays.
		 */
		uint8_t getDisplayCount() const;

		/** Returns the display name for the given display index.
		 */
		std::string getDisplayName(uint8_t display = 0) const;

		/** Returns the SDL_PixelFormatEnum of the desktop for the given display index.
		 */
		uint32_t getDesktopFormat(uint8_t display = 0) const;
		
		/** Returns the refresh rate in Hz of the desktop for the given display index.
		 */
		int32_t getDesktopRefreshRate(uint8_t display = 0) const;

		/** Returns the width of the desktop resolution for the given display index.
		 */
		int32_t getDesktopWidth(uint8_t display = 0) const;

		/** Returns the height of the desktop resolution for the given display index.
		 */
		int32_t getDesktopHeight(uint8_t display = 0) const;

		/** Returns the bounding points for the given display index.
		 */
		Rect getDisplayBounds(uint8_t display = 0) const;

	private:
		std::vector<ScreenMode> m_screenModes;
		std::string m_videoDriverName;
		std::vector<std::string> m_availableVideoDrivers;

		std::string m_renderDriverName;
		int8_t m_renderDriverIndex;
		std::vector<std::string> m_availableRenderDrivers;

		/** Called in the constructor.  No need for anyone to call this
		 */
		void fillAvailableDrivers();
	}; //DeviceCaps
} //FIFE



#endif //FIFE_DEVICECAPS_H
