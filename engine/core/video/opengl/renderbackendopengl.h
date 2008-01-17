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

#ifndef FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H
#define FIFE_VIDEO_RENDERBACKENSD_OPENGL_RENDERBACKENDOPENGL_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "video/renderbackend.h"

namespace FIFE {

	class GLScreen;

	/** The main class of the OpenGL-based renderer.
	 *
	 * @see RenderBackend
	 */
	class RenderBackendOpenGL : public RenderBackend {
		public:
			/** Simple constructor.
			 *
			 * @note Registers the renderbackend 'OpenGL' but does not yet create a screen.
			 */
			RenderBackendOpenGL();

			/** Deletes the backend instance.
			 */
			virtual ~RenderBackendOpenGL();

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
			 * @note Hardcoded flags: SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_HWACCEL
			 */
			virtual SDL_Surface* createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs);

			/** Convenience wrapper around GLImage.
			 *
			 * Creates an GLImage instance from the specified data.
			 *
			 * @see GLImage
			 */
			virtual Image* createStaticImageFromSDL(SDL_Surface* surface);

			/** Makes a screenshot and saves it as a BMP file.
			 */
			virtual void captureScreen(const std::string& filename);

			virtual void drawLine(const Point& p1, const Point& p2, int r, int g, int b);
			virtual void drawQuad(const Point& p1, const Point& p2, const Point& p2, const Point& p3,  int r, int g, int b);

		protected:
			void setClipArea(const Rect& cliparea);

		private:
			SDL_PixelFormat m_rgba_format;
	};

}

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
