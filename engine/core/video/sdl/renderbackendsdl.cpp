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

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/math/fife_math.h"
#include "util/log/logger.h"

#include "renderbackendsdl.h"
#include "sdlimage.h"
#include "SDL_image.h"

namespace FIFE {
	static Logger _log(LM_VIDEO);

	RenderBackendSDL::RenderBackendSDL() : RenderBackend() {
	}


	RenderBackendSDL::~RenderBackendSDL() {
		deinit();
	}

	const std::string& RenderBackendSDL::getName() const {
		static std::string backend_name = "SDL";
		return backend_name;
	}
	
	void RenderBackendSDL::init() {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
			throw SDLException(SDL_GetError());

		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL); // temporary hack
	}

	Image* RenderBackendSDL::createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs, const std::string& title, const std::string& icon) {
		Uint32 flags = 0;
		if (fs) {
			flags |= SDL_FULLSCREEN;
		}

		if(icon != "") {
			SDL_Surface *img = IMG_Load(icon.c_str());
			if(img != NULL) {
				SDL_WM_SetIcon(img, 0);
			}
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
		FL_LOG(_log, LMsg("RenderBackendSDL")
			<< "Videomode " << width << "x" << height
			<< " at " << int(screen->format->BitsPerPixel) << " bpp");

		SDL_WM_SetCaption(title.c_str(), NULL);

		if (!screen) {
			throw SDLException(SDL_GetError());
		}

		m_screen = new SDLImage(screen);
		return m_screen;
	}

	void RenderBackendSDL::startFrame() {
		SDL_FillRect(m_screen->getSurface(), 0, 0x00);
	}

	void RenderBackendSDL::endFrame() {
		SDL_Flip(m_screen->getSurface());
	}	

	Image* RenderBackendSDL::createImage(SDL_Surface* surface) {
		return new SDLImage(surface);
	}

	Image* RenderBackendSDL::createImage(const uint8_t* data, unsigned int width, unsigned int height) {
		return new SDLImage(data, width, height);
	}
	
	bool RenderBackendSDL::putPixel(int x, int y, int r, int g, int b) {
		return static_cast<SDLImage*>(m_screen)->putPixel(x, y, r, g, b);
	}
	
	void RenderBackendSDL::drawLine(const Point& p1, const Point& p2, int r, int g, int b) {
		static_cast<SDLImage*>(m_screen)->drawLine(p1, p2, r, g, b);
	}
	
	void RenderBackendSDL::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b) {
		static_cast<SDLImage*>(m_screen)->drawQuad(p1, p2, p3, p4, r, g, b);
	}
}
