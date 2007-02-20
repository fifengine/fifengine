/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_VIDEO_RENDERBACKEND_H
#define FIFE_VIDEO_RENDERBACKEND_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	class Screen;
	class Image;

	 /** Abstract interface for all the renderbackends. */
	class RenderBackend {
		public:
			/** Constructor.
			 *
			 * @param name The name of the new renderbackend.				
			 */
			RenderBackend(const std::string& name);
			/** Destructor.
			 */
			virtual ~RenderBackend();

			/** The name of the renderbackend.
			 *
			 * @return The name of this renderbackend.
			 */
			const std::string& getName() const;

			/** Called when a new frame starts.
			 */
			virtual void startFrame() = 0;

			/** Called when a frame is finished and ready to be displayed.
			 */
			virtual void endFrame() = 0;

			/** Initializes the backend.
			 */
			virtual void init() = 0;

			/** Performs cleanup actions.
			 */
			virtual void deinit() = 0;

			/** Creates the mainscreen (the display window).
			 *
			 * @param width Width of the window.
			 * @param height Height of the window.
			 * @param bitsPerPixel Bits per pixel, 0 means autodetect.
			 * @param fullscreen Use fullscreen mode?
			 * @return The new Screen.
			 */
			virtual Screen* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fullscreen) = 0;

			/** Creates an Image suitable for this renderbackend.
			 *
			 * @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
			 * @param width Width of the image.
			 * @param height Height of the image.
			 * @return The new Image.
			 * @see createStaticImageFromSDL()
			 */
			virtual Image* createStaticImageFromRGBA(const uint8_t* data, unsigned int width, unsigned int height);

			/** Helper function to create images from SDL_Surfaces.
			 *
			 * It just converts the surface into a suitable format for createStaticImageFromRGBA().
			 *
			 * @param surface The surface to convert.
			 * @param freesurface If true, the surface will be freed.
			 * @return The new Image.
			 * @see createStaticImageFromRGBA()
			 */
			virtual Image* createStaticImageFromSDL(SDL_Surface* surface, bool freesurface = true) = 0;

			/** Gets the current MainScreen (the displaywindow).
			 *
			 * @return The mainscreen, or 0 if no mainscreen exists.
			 * @see createMainScreen()
			 */
			virtual Screen* getMainScreen() const = 0;

			/** Creates a Screenshot and saves it to a file.
			 *
			 * @see Engine::makeScreenshot()
			 */
			virtual void captureScreen(const std::string& filename);
			

		private:
			// The name of the renderbackend.
			std::string m_name;
	};

}

#endif
