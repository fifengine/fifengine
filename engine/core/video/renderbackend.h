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
#include <stack>

// Platform specific includes
#include "util/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/singleton.h"
#include "util/point.h"
#include "util/rect.h"

namespace FIFE {

	class Image;

	 /** Abstract interface for all the renderbackends. */
	class RenderBackend: public DynamicSingleton<RenderBackend> {
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
			 * @return The new Screen SDL_Surface surface.
			 */
			virtual SDL_Surface* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fullscreen) = 0;

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
			 * Takes ownership over the surface.
			 * @param surface The surface to convert.
			 * @return The new Image.
			 * @see createStaticImageFromRGBA()
			 */
			virtual Image* createStaticImageFromSDL(SDL_Surface* surface) = 0;

			/** Returns a pointer to the main screen SDL surface.
			 * @return A pointer to the main screen SDL surface, or 0 if no mainscreen exists.
			 */
			SDL_Surface* getScreenSurface() const { return m_screen; };

			/** Gets the width of the screen.
			 *
			 * @return Width of the screen.
			 */
			unsigned int getScreenWidth() const { return m_screen->w; }

			/** Gets the area of the screen
			 *
			 * @return Screen area in rect
			 */
			const Rect& getScreenArea();

			/** Gets the height of the screen.
			 *
			 * @return Height of the screen.
			 */
			unsigned int getScreenHeight() const { return m_screen->h; }

			/** Creates a Screenshot and saves it to a file.
			 *
			 * @see Engine::makeScreenshot()
			 */
			virtual void captureScreen(const std::string& filename);

			/** Draws line between given point with given color (rgb)
			 */
			virtual void drawLine(const Point& p1, const Point& p2, int r, int g, int b) = 0;
			virtual void drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4, int r, int g, int b) =0;

			/** Pushes clip area to clip stack
			 *  Clip areas define which area is drawn on screen. Usable e.g. with viewports
			 *  note that previous items in stack do not affect the latest area pushed
			 */
			void pushClipArea(const Rect& cliparea);

			/** Pops clip area from clip stack
			 *  @see pushClipArea
			 */
			void popClipArea();

			/** Gets the current clip area
			 *  @see pushClipArea
			 */
			const Rect& getClipArea() const;


		protected:
			/** Sets given clip area to render backend
			 *  @see pushClipArea
			 */
			virtual void setClipArea(const Rect& cliparea) = 0;

			/** Clears any possible clip areas
			 *  @see pushClipArea
			 */
			virtual void clearClipArea();

			SDL_Surface* m_screen;

		private:
			Rect m_screenarea;
			// The name of the renderbackend.
			std::string m_name;
			std::stack<Rect> m_clipstack;
	};

}

#endif
