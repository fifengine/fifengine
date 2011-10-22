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

#ifndef FIFE_ENGINESETTINGS_H
#define FIFE_ENGINESETTINGS_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

namespace FIFE {
	class NotSupported;

	/** This class defines the engine settings on engine init
	 *
	 */
	class EngineSettings {
	public:
		/** Constructor
		 */
		EngineSettings();

		/** Destructor
		 */
		~EngineSettings();

		/** Sets bits per pixel
		 * @see getPossibleBitsPerPixel
		 */
		void setBitsPerPixel(uint8_t bitsperpixel);

		/** Gets currently set bits per pixel value
		 */
		uint8_t getBitsPerPixel() const {
			return m_bitsperpixel;
		}

		/** Gets all possible bits per pixel values
		 */
		std::vector<uint8_t> getPossibleBitsPerPixel() const;

		/** Sets fullscreen / windowed mode
		 */
		void setFullScreen(bool fullscreen) {
			m_fullscreen = fullscreen;
		}

		/** True, if set to fullscreen. False = windowed
		 */
		bool isFullScreen() const {
			return m_fullscreen;
		}

		/** Sets initial engine sound volume
		 *  @see getInitialMaxVolume
		 */
		void setInitialVolume(float volume);

		/** Gets initial engine sound volume
		 */
		float getInitialVolume() const {
			return m_initialvolume;
		}

		/** Gets maximum volume that can be set
		 */
		float getMaxVolume() const;

		/** Sets name for renderbackend
		 *  @see getPossibleRenderBackends
		 */
		void setRenderBackend(const std::string& renderbackend);

		/** Gets currently set renderbackend name
		 */
		const std::string& getRenderBackend() const {
			return m_renderbackend;
		}

		/** Gets currently set renderbackend name
		 */
		std::string getRenderBackend() {
			return m_renderbackend;
		}

		/** Gets all possible renderbackend names
		 */
		std::vector<std::string> getPossibleRenderBackends();

		/** Sets if fake alpha is removed in SDL renderbackend
		 */
		void setSDLRemoveFakeAlpha(bool sdlremovefakealpha);

		/** Tells if fake alpha is removed in SDL renderbackend
		 */
		bool isSDLRemoveFakeAlpha() const {
			return m_sdlremovefakealpha;
		}

		/** Sets if images are compress by video driver in OpenGL renderbackend
		*/
		void setGLCompressImages(bool oglcompressimages);

		/** Tells if images are compress by video driver in OpenGL renderbackend
		*/
		bool isGLCompressImages() const {
			return m_oglcompressimages;
		}

		/** Sets if OpenGL renderbackend should use FramebufferObject (when available)
		*/
		void setGLUseFramebuffer(bool ogluseframebuffer);

		/** Tells if OpenGL renderbackend should use FramebufferObject
		*/
		bool isGLUseFramebuffer() const {
			return m_ogluseframebuffer;
		}

		/** Sets screen width (pixels)
		 */
		void setScreenWidth(uint16_t screenwidth);

		/** Gets screen width (pixels)
		 */
		uint16_t getScreenWidth() const {
			return m_screenwidth;
		}

		/** Sets screen height (pixels)
		 */
		void setScreenHeight(uint16_t screenheight);

		/** Gets screen height (pixels)
		 */
		uint16_t getScreenHeight() const {
			return m_screenheight;
		}

		/** Sets path for default font
		 */
		void setDefaultFontPath(const std::string& defaultfontpath);

		/** Gets current path for default font
		 */
		const std::string& getDefaultFontPath() const {
			return m_defaultfontpath;
		}

		/** Gets current path for default font
		 */
		std::string getDefaultFontPath() {
			return m_defaultfontpath;
		}

		/** Sets size for default font
		 */
		void setDefaultFontSize(uint16_t defaultfontsize);

		/** Gets size for default font
		 */
		uint16_t getDefaultFontSize() const {
			return m_defaultfontsize;
		}

		/** Sets glyphs for default font
		 */
		void setDefaultFontGlyphs(const std::string& defaultfontglyphs);

		/** Gets current glyphs for default font
		 */
		const std::string& getDefaultFontGlyphs() const {
			return m_defaultfontglyphs;
		}

				/** Gets current glyphs for default font
		 */
		std::string getDefaultFontGlyphs() {
			return m_defaultfontglyphs;
		}

		/** Sets the title of the window
		 */
		void setWindowTitle(const std::string& title);

		/** Gets the current window title
		 */
		const std::string& getWindowTitle() const {
			return m_windowtitle;
		}

		/** Gets the current window title
		 */
		std::string getWindowTitle() {
			return m_windowtitle;
		}

		/** Sets the icon that appears in the window title bar
		 */
		void setWindowIcon(const std::string& icon);

		/** Gets the icon in the window title bar
		 */
		const std::string& getWindowIcon() const {
			return m_windowicon;
		}

		/** Gets the icon in the window title bar
		 */
		std::string getWindowIcon() {
			return m_windowicon;
		}

		/** Sets whether to use the colorkey feature
		 */
		void setColorKeyEnabled(bool colorkeyenable);

		/** Gets whether the colorkey feature is in use
		 */
		bool isColorKeyEnabled() const;

		/** Sets the global colorkey to use for images
		 */
		void setColorKey(uint8_t r, uint8_t g, uint8_t b);

		/** Gets the global colorkey setting
		 */
		const SDL_Color& getColorKey() const;

		void setVideoDriver(const std::string& driver);

		const std::string& getVideoDriver() const;

		/** Sets the light model
		 */
		void setLightingModel(uint32_t lighting);

		/** Gets the currently set light model
		 */
		uint32_t getLightingModel() const {
			return m_lighting;
		}

		/** Sets whether to use the frame limiter
		 */
		void setFrameLimitEnabled(bool limited);
		
		/** Gets whether the frame limiter is in use
		 */
		bool isFrameLimitEnabled() const;

		/** Sets the frame limit
		 */
		void setFrameLimit(uint16_t framelimit);

		/** Gets the frame limit
		 */
		uint16_t getFrameLimit() const;

		/** Sets mouse sensitivity
		 */
		void setMouseSensitivity(float sens);

		/** Gets mouse sensitivity
		 */
		float getMouseSensitivity() const;

		/** Sets mouse acceleration
		 * if mouse acceleration is enabled,
		 * then the mouse sensitivity is used as speed max.
		 */
		void setMouseAcceleration(bool acceleration);

		/** Gets mouse acceleration
		 */
		bool getMouseAcceleration() const;

	private:
		uint8_t m_bitsperpixel;
		bool m_fullscreen;
		float m_initialvolume;
		std::string m_renderbackend;
		bool m_sdlremovefakealpha;
		bool m_oglcompressimages;
		bool m_ogluseframebuffer;
		uint16_t m_screenwidth;
		uint16_t m_screenheight;
		std::string m_windowtitle;
		std::string m_windowicon;


		std::string m_defaultfontpath;
		uint16_t m_defaultfontsize;
		std::string m_defaultfontglyphs;
		bool m_iscolorkeyenabled;
		SDL_Color m_colorkey;
		std::string m_videodriver;
		uint32_t m_lighting;
		bool m_isframelimit;
		uint16_t m_framelimit;
		float m_mousesensitivity;
		bool m_mouseacceleration;
	};

}//FIFE

#endif

