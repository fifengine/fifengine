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

// Standard C++ library includes

// Platform specific includes
#include "util/fife_opengl.h"

// 3rd party library includes
#include <guichan/opengl.hpp>
#include <guichan/sdl.hpp>
#include <guichan.hpp>
#include <SDL.h>

// FIFE includes
#include "video/gui/guimanager.h"
#include "debugutils.h"
#include "exception.h"

#include "glimage.h"
#include "glscreen.h"
#include "renderbackendopengl.h"

namespace FIFE {

	RenderBackendOpenGL::RenderBackendOpenGL() : RenderBackend("OpenGL"), m_screen(0){
		// Get the pixelformat we want.
		SDL_Surface* testsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 1, 1, 32,
				0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

		m_rgba_format = *(testsurface->format);
		SDL_FreeSurface(testsurface);
	}


	RenderBackendOpenGL::~RenderBackendOpenGL() {
		delete m_screen;
	}


	void RenderBackendOpenGL::init() {
		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0)
			throw SDLException(SDL_GetError());

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	void RenderBackendOpenGL::deinit() {
		delete m_screen;
		m_screen = 0;

		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	Screen* RenderBackendOpenGL::createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs) {
		delete m_screen;
		m_screen = 0;

		Uint32 flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_HWACCEL;
		if ( fs ) {
			flags |= SDL_FULLSCREEN;
		}

		SDL_Surface* screen = NULL;

		if( 0 == bitsPerPixel ) {
			/// autodetect best mode
			unsigned char possibleBitsPerPixel[] = {32, 24, 16, 0};
			int i = 0;
			while( true ) {
				bitsPerPixel = possibleBitsPerPixel[i];
				if( !bitsPerPixel ) {
					throw SDLException("Videomode not available");
				}

				if ( SDL_VideoModeOK(width, height, bitsPerPixel, flags) ) {
					screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
					if( screen ) {
						break;
					}
				}
				++i;
			}
		} else {
			if ( !SDL_VideoModeOK(width, height, bitsPerPixel, flags) ) {
				throw SDLException("Videomode not available");
			}
			screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
		}

		SDL_WM_SetCaption("FIFE", 0);

		if (!screen) {
			throw SDLException(SDL_GetError());
		}

		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		gluOrtho2D(0, width, height, 0);
		glMatrixMode(GL_MODELVIEW);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_screen = new GLScreen();
		GUIManager* guimanager = GUIManager::instance();
		guimanager->init();

		return m_screen;
	}

	void RenderBackendOpenGL::startFrame() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderBackendOpenGL::endFrame() {
		SDL_GL_SwapBuffers();
	}

	Image* RenderBackendOpenGL::createStaticImageFromRGBA(const uint8_t* data, unsigned int width, unsigned int height) {
		return new GLImage(data, width, height);
	}
	
	Image* RenderBackendOpenGL::createStaticImageFromSDL(SDL_Surface* surface, bool freesurface) {
		// Given an abritary surface, we must convert it to the format GLImage will understand.
		// It's easiest to let SDL do this for us.
		// We may want to check if the surface already is in the correct form, to avoid unneeded
		// copying.
		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE);
		if (freesurface) {
			SDL_FreeSurface(surface);
		}
		if (conv != 0) {
			SDL_LockSurface(conv);
			Image* image = new GLImage(static_cast<const uint8_t*>(conv->pixels), conv->w, conv->h, conv->pitch);
			SDL_UnlockSurface(conv);
			SDL_FreeSurface(conv);
			return image;
		}
	
		return 0;
	}

	Screen* RenderBackendOpenGL::getMainScreen() const {
		return m_screen;
	}

	void RenderBackendOpenGL::captureScreen(const std::string& filename) {
		unsigned int swidth = getMainScreen()->getWidth();
		unsigned int sheight = getMainScreen()->getHeight();

		// We need unsigned char to avoid pointer alignment issues
		uint8_t *pixels = new uint8_t[swidth * sheight * 3];

		// Read in the pixel data
		glReadPixels(0, 0, swidth, sheight, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

		/* At this point the image has been reversed, so we need to re-reverse it so that
		it is the correct way around. We do this by copying the "image" pixels to another
		surface in reverse order */
		SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth, sheight, 24,
			255U << (0), // Blue channel
			255U << (8), // Green channel
			255U << (16), // Red channel
			0 /* no alpha! */);
		SDL_LockSurface(image);

		uint8_t *imagepixels = reinterpret_cast<uint8_t*>(image->pixels);
		// Copy the "reversed_image" memory to the "image" memory
		for (int y = (sheight - 1); y >= 0; --y) {
			uint8_t *row_begin = pixels + y * swidth * 3;
			uint8_t *row_end = row_begin + swidth * 3;

			std::copy(row_begin, row_end, imagepixels);

			// Advance a row in the output surface.
			imagepixels += image->pitch;
		}
		SDL_UnlockSurface(image);

		// Save file
		SDL_SaveBMP(image, filename.c_str());

		// Clear memory
		delete []pixels;
		SDL_FreeSurface( image );
	}
}
