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
		/** Constructors.
		 */
		ScreenMode();
		ScreenMode(uint16_t width, uint16_t height, uint16_t bpp, uint32_t SDLFlags);
		ScreenMode(const ScreenMode& rhs);

		/** Destructor.
		 */
		~ScreenMode() {};

		/** Returns the width of the screen mode.
		 */
		uint16_t getWidth() const { return m_width; };

		/** Returns the height of the screen mode.
		 */
		uint16_t getHeight() const { return m_height; };

		/** Returns the number of bits per pixel this mode supports.
		 */
		uint16_t getBPP() const { return m_bpp; };

		/** Returns the SDL flags used when testing this mode.
		 */
		uint32_t getSDLFlags() const { return m_SDLFlags; };

		/** True if this is a fullscreen mode.  Fals if it is a windowed mode.
		 */
		bool isFullScreen() const { return (m_SDLFlags & SDL_FULLSCREEN) ? true : false;};

		/** True if this mode uses the OpenGL renderer.  False otherwise.
		 */
		bool isOpenGL() const { return (m_SDLFlags & SDL_OPENGL) ? true : false; };

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

		void fillDeviceCaps();
		std::vector<ScreenMode> getSupportedScreenModes() const {return m_screenModes;} ;

	private:
		std::vector<ScreenMode> m_screenModes;

	}; //DeviceCaps
}

#endif //FIFE_DEVICECAPS_H
