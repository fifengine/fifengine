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

// 3rd party library includes
#include <SDL.h>

// FIFE includes
#include "util/exception.h"

#include "fife_opengl.h"
#include "glimage.h"
#include "renderbackendopengl.h"

namespace FIFE {

	RenderBackendOpenGL::RenderBackendOpenGL() : RenderBackend() {
		// Get the pixelformat we want.
		SDL_Surface* testsurface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, 1, 1, 32,
				RMASK, GMASK, BMASK ,AMASK);

		m_rgba_format = *(testsurface->format);
		SDL_FreeSurface(testsurface);
	}

	const std::string& RenderBackendOpenGL::getName() const {
		static std::string backend_name = "OpenGL";
		return backend_name;
	}

	RenderBackendOpenGL::~RenderBackendOpenGL() {
	}


	void RenderBackendOpenGL::init() {
		Uint32 flags = SDL_INIT_VIDEO;
		if (SDL_InitSubSystem(flags) < 0)
			throw SDLException(SDL_GetError());

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	Image* RenderBackendOpenGL::createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs) {
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

		glEnable(GL_SCISSOR_TEST);

		glPointSize(1.0);
		glLineWidth(1.0);

		m_screen = new GLImage(screen);
		return m_screen;
	}

	void RenderBackendOpenGL::startFrame() {
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void RenderBackendOpenGL::endFrame() {
		SDL_GL_SwapBuffers();
	}

	Image* RenderBackendOpenGL::createImage(SDL_Surface* surface) {
		// Given an abritary surface, we must convert it to the format GLImage will understand.
		// It's easiest to let SDL do this for us.

		// Uh. Gotta love this :-)
		// Check for colorkey too?
		// Leave out the loss/shift checks?
		if(    m_rgba_format.BitsPerPixel == surface->format->BitsPerPixel
			&& m_rgba_format.Rmask == surface->format->Rmask
			&& m_rgba_format.Gmask == surface->format->Gmask
			&& m_rgba_format.Bmask == surface->format->Bmask
			&& m_rgba_format.Amask == surface->format->Amask
			&& m_rgba_format.Rshift == surface->format->Rshift
			&& m_rgba_format.Gshift == surface->format->Gshift
			&& m_rgba_format.Bshift == surface->format->Bshift
			&& m_rgba_format.Ashift == surface->format->Ashift
			&& m_rgba_format.Rloss == surface->format->Rloss
			&& m_rgba_format.Gloss == surface->format->Gloss
			&& m_rgba_format.Bloss == surface->format->Bloss
			&& m_rgba_format.Aloss == surface->format->Aloss
			&& surface->flags & SDL_SRCALPHA   ) {

			return new GLImage(surface);
		}

		SDL_Surface* conv = SDL_ConvertSurface(surface, &m_rgba_format, SDL_SWSURFACE|SDL_SRCALPHA);
		GLImage* image = new GLImage(conv);
		SDL_FreeSurface( surface );
		return image;
	}

	Image* RenderBackendOpenGL::createImage(const uint8_t* data, unsigned int width, unsigned int height) {
		return new GLImage(data, width, height);
	}
}
