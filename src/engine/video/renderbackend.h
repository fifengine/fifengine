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

#ifndef FIFERENDERBACKEND_H
#define FIFERENDERBACKEND_H

#include <string>
#include <stdint.h>

#include <SDL_video.h>

namespace FIFE {

	class Screen;
	class Image;

	 /** Abstract interface for all renderbackends */
	class RenderBackend {
		public:
			RenderBackend(const std::string& name);
			virtual ~RenderBackend();

			/** RenderBackend-name
			 *
			 * @return the name of this RenderBackend;
			 */
			const std::string& getName() const;

			/** called when a new frame starts */
			virtual void startFrame() = 0;

			/** called when a frame is finished and ready do be displayd */
			virtual void endFrame() = 0;

			/** initialize the backend */
			virtual void init() = 0;

			/** perform cleanup-actions*/
			virtual void deinit() = 0;

			/** Creates the mainscreen (the displaywindow)
			 *
			 * @param width the width of the window
			 * @param height the height of the window
			 * @param bitsPerPixel bits per pixel, 0 - means autodetect
			 * @param fullscreen use fullscreen mode?
			 * @return the new Screen
			 */
			virtual Screen* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fullscreen) = 0;

			/** Creates a Image suitable for this RenderBackend
			 *
			 * @param data pointer to the imagedata (needs to be in RGBA, 8bit per channel)
			 * @param width the width of the image
			 * @param height the height of the image
			 * @return the new Image
			 * @see createStaticImageFromSDL
			 */
			virtual Image* createStaticImageFromRGBA(const uint8_t* data, unsigned int width, unsigned int height) = 0;

			/** Helper function to create images from SDL_Surfaces
			 *
			 * It just converts the surface into a suitable format for createStaticImageFromRGBA.
			 * @param surface the surface to convert.
			 * @param freesurface if true surface will be freed
			 * @return the new Image
			 * @see createStaticImageFromRGBA
			 */
			Image* createStaticImageFromSDL(SDL_Surface* surface, bool freesurface = true);

			/** Get the current MainScreen (the displaywindow)
			 *
			 * @return the mainscreen, or 0 if no mainscreen exists.
			 * @see createMainScreen
			 */
			virtual Screen* getMainScreen() const = 0;

			/** Create a Screenshot and save it to a file.
			 * @see Engine::makeScreenshot
			 */
			virtual void captureScreen(const std::string& filename);
			

		private:
			std::string m_name;
	};

}

#endif
