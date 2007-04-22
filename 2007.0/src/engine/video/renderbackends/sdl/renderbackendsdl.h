/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_RENDERBACKENDSDL_H
#define FIFE_RENDERBACKENDSDL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "video/renderbackend.h"

namespace FIFE {

	class SDLScreen;

	/** The main class of the SDL-based renderer.
	 *
	 * @see RenderBackend
	 */
	class RenderBackendSDL : public RenderBackend {
		public:
			/** Simple constructor.
			 *
			 * @note Registers the renderbackend 'SDL' but does not yet create a screen.
			 */
			RenderBackendSDL();

			/** Deletes the backend instance.
			 */
			virtual ~RenderBackendSDL();

			/* Starts the frame.
			 */
			virtual void startFrame();
			/* Ends the frame.
			 */
			virtual void endFrame();
			
			/** Initialises the SDL video subsystem.
			 */
			virtual void init();

			/** Deletes the current screen and quits the SDL video subsystem.
			 */
			virtual void deinit();

			/** Creates the internal screen and returns it as well.
			 *
			 * @note Use sensible values; fs means fullscreen.
			 * @note bitsPerPixel = 0 causes autodetection of best supported mode.
			 */
			virtual Screen* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs);

			/** Convenience wrapper around SDLImage.
			 *
			 * Creates an SDLImage instance from the specified data.
			 * 
			 * @see SDLImage
			 */
			virtual Image* createStaticImageFromRGBA(const uint8_t* data, unsigned int width, unsigned int height);
			
			/** Returns the current screen.
			 *
			 * @note This may be NULL.
			 */
			virtual Screen* getMainScreen() const;

			/** Makes a screenshot and saves it as a BMP file.
			 */
			virtual void captureScreen(const std::string& filename);
		
		private:
			// SDL screen.
			SDLScreen* m_screen;
	};

}

#endif
