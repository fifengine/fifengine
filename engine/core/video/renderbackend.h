/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_VIDEO_RENDERBACKEND_H
#define FIFE_VIDEO_RENDERBACKEND_H

// Standard C++ library includes
#include <string>

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes
#include <SDL.h>
#include <SDL_video.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"

#include "image.h"

namespace FIFE {

	class Image;

	 /** Abstract interface for all the renderbackends. */
	class RenderBackend: public AbstractImage, public DynamicSingleton<RenderBackend> {
	public:
		/** Constructor.
		 * @param name The name of the new renderbackend.
		 */
		RenderBackend();
		
		/** Destructor.
		 */
		virtual ~RenderBackend();

		/** The name of the renderbackend.
		 * @return The name of this renderbackend.
		 */
		virtual const std::string& getName() const = 0;

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
		virtual void deinit();

		/** Creates the mainscreen (the display window).
		 * @param width Width of the window.
		 * @param height Height of the window.
		 * @param bitsPerPixel Bits per pixel, 0 means autodetect.
		 * @param fullscreen Use fullscreen mode?
		 * @return The new Screen Image
		 */
		virtual Image* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fullscreen, const std::string& title, const std::string& icon) = 0;

		/** Creates an Image suitable for this renderbackend.
		 * @param data Pointer to the imagedata (needs to be in RGBA, 8 bits per channel).
		 * @param width Width of the image.
		 * @param height Height of the image.
		 * @return The new Image.
		 */
		virtual Image* createImage(const uint8_t* data, unsigned int width, unsigned int height) = 0;
		
		/** Helper function to create images from SDL_Surfaces.
		 * Takes ownership over the surface.
		 * @param surface The surface to convert.
		 * @return The new Image.
		 */
		virtual Image* createImage(SDL_Surface* surface) = 0;
		
		/** Returns a pointer to the main screen Image
		 * @return A pointer to the main screen Image, or 0 if no mainscreen exists.
		 */
		Image* getScreenImage() const { return m_screen; };

		/** Creates a Screenshot and saves it to a file.
		 */
		void captureScreen(const std::string& filename);
		
		SDL_Surface* getSurface();
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		unsigned int getScreenWidth() const { return getWidth(); }
		unsigned int getScreenHeight() const { return getHeight(); }
		const Rect& getArea();
		void getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);
		void pushClipArea(const Rect& cliparea, bool clear=true);
		void popClipArea();
		const Rect& getClipArea() const;
		void setAlphaOptimizerEnabled(bool enabled);
		bool isAlphaOptimizerEnabled();
		void saveImage(const std::string& filename);

		/** OpenGL image needs to be sliced into power of two sized chunks. 
		 * You can adjust the size by calling this method.
		 * Size can be anything (reasonable), it is always changed to the next biggest power of two value
		 * @example values 50 -> 64, 340 -> 512
		 */
		void setChunkingSize(unsigned int size);
		unsigned int getChunkingSize();

	protected:
		Image* m_screen;
		bool m_isalphaoptimized;
		unsigned int m_chunkingsize;
	};
}

#endif
