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

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"
#include "util/log.h"

#include "renderbackendsdl.h"
#include "sdlimage.h"
#include "sdlscreen.h"

namespace FIFE {

	RenderBackendSDL::RenderBackendSDL() : RenderBackend("SDL"), m_screen(0) {

	}


	RenderBackendSDL::~RenderBackendSDL() {
		deinit();
	}

	void RenderBackendSDL::init() {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
			throw SDLException(SDL_GetError());

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	void RenderBackendSDL::deinit() {
		delete m_screen;
		m_screen = 0;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	Screen* RenderBackendSDL::createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs) {
		Uint32 flags = 0;
		if (fs) {
			flags |= SDL_FULLSCREEN;
		}

		SDL_Surface* screen = NULL;

		if( 0 == bitsPerPixel ) {
			/// autodetect best mode
			unsigned char possibleBitsPerPixel[] = {16, 24, 32, 0};
			int i = 0;
			while( true ) {
				bitsPerPixel = possibleBitsPerPixel[i];
				if( !bitsPerPixel ) {
					// Last try, sometimes VideoModeOK seems to lie.
					// Try bpp=0
					screen = SDL_SetVideoMode(width, height, bitsPerPixel, flags);
					if( !screen ) {
						throw SDLException("Videomode not available");
					}
					break;
				}
				bitsPerPixel = SDL_VideoModeOK(width, height, bitsPerPixel, flags);
				if ( bitsPerPixel ) {
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

		Log("RenderBackendSDL")
			<< "Videomode " << width << "x" << height
			<< " at " << int(screen->format->BitsPerPixel) << " bpp";

		SDL_WM_SetCaption("FIFE", NULL);

		if (!screen) {
			throw SDLException(SDL_GetError());
		}

		m_screen = new SDLScreen(screen);
		return m_screen;
	}

	void RenderBackendSDL::startFrame() {
		SDL_FillRect(m_screen->getSurface(), 0, 0x00);
	}

	void RenderBackendSDL::endFrame() {
		SDL_Flip(m_screen->getSurface());
	}	

	Image* RenderBackendSDL::createStaticImageFromSDL(SDL_Surface* surface) {
		return new SDLImage(surface);
	}

	Screen* RenderBackendSDL::getMainScreen() const {
		return m_screen;
	}

	void RenderBackendSDL::captureScreen(const std::string& filename) {
		if( m_screen && m_screen->getSurface() ) {
			SDL_SaveBMP(m_screen->getSurface(),filename.c_str());
		}
	}
}
