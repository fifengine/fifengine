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
#include "util/fife_math.h"
#include "util/logger.h"

#include "renderbackendsdl.h"
#include "sdlimage.h"

namespace FIFE {
	static Logger _log(LM_VIDEO);

	RenderBackendSDL::RenderBackendSDL() : RenderBackend("SDL"), m_removefakealpha(true) {

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
		SDL_FreeSurface(m_screen);
		m_screen = 0;
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}

	SDL_Surface* RenderBackendSDL::createMainScreen(unsigned int width, unsigned int height, unsigned char bitsPerPixel, bool fs) {
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
		FL_LOG(_log, LMsg("RenderBackendSDL")
			<< "Videomode " << width << "x" << height
			<< " at " << int(screen->format->BitsPerPixel) << " bpp");

		SDL_WM_SetCaption("FIFE", NULL);

		if (!screen) {
			throw SDLException(SDL_GetError());
		}

		m_screen = screen;
		return m_screen;
	}

	void RenderBackendSDL::startFrame() {
		SDL_FillRect(m_screen, 0, 0x00);
	}

	void RenderBackendSDL::endFrame() {
		SDL_Flip(m_screen);
	}	

	Image* RenderBackendSDL::createStaticImageFromSDL(SDL_Surface* surface) {
		return new SDLImage(surface);
	}

	void RenderBackendSDL::captureScreen(const std::string& filename) {
		if( m_screen ) {
 			SDL_SaveBMP(m_screen,filename.c_str());
		}
	}

	inline void RenderBackendSDL::putPixel(int x, int y, int r, int g, int b)
	{
		if ((x < 0) || (x >= m_screen->w) || (y < 0) || (y >= m_screen->h)) {
			return;
		}
		int bpp = m_screen->format->BytesPerPixel;
		SDL_LockSurface(m_screen);
		Uint8 *p = (Uint8 *)m_screen->pixels + y * m_screen->pitch + x * bpp;
		Uint32 pixel = SDL_MapRGB(m_screen->format, r, g, b);
		switch(bpp)
		{
			case 1:
				*p = pixel;
				break;
		
			case 2:
				*(Uint16 *)p = pixel;
				break;
		
			case 3:
				if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				}
				else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;
		
			case 4:
				*(Uint32 *)p = pixel;
				break;
		}
		SDL_UnlockSurface(m_screen);
	}


	void RenderBackendSDL::drawLine(const Point& p1, const Point& p2, int r, int g, int b) {
		// Draw a line with Bresenham, imitated from guichan
		int x1 = p1.x;
		int x2 = p2.x;
		int y1 = p1.y;
		int y2 = p2.y;
		int dx = ABS(x2 - x1);
		int dy = ABS(y2 - y1);
	
		if (dx > dy) {
			if (x1 > x2) {
				// swap x1, x2
				x1 ^= x2;
				x2 ^= x1;
				x1 ^= x2;
		
				// swap y1, y2
				y1 ^= y2;
				y2 ^= y1;
				y1 ^= y2;
			}
	
			if (y1 < y2) {
				int y = y1;
				int p = 0;
		
				for (int x = x1; x <= x2; x++) {
					putPixel(x, y, r, g, b);
					p += dy;
					if (p * 2 >= dx) {
						y++;
						p -= dx;
					}
				}
			}
			else {
				int y = y1;
				int p = 0;
		
				for (int x = x1; x <= x2; x++) {
					putPixel(x, y, r, g, b);
			
					p += dy;
					if (p * 2 >= dx) {
						y--;
						p -= dx;
					}
				}
			}
		}
		else {
			if (y1 > y2) {
				// swap y1, y2
				y1 ^= y2;
				y2 ^= y1;
				y1 ^= y2;
		
				// swap x1, x2
				x1 ^= x2;
				x2 ^= x1;
				x1 ^= x2;
			}
	
			if (x1 < x2) {
				int x = x1;
				int p = 0;
		
				for (int y = y1; y <= y2; y++) {
					putPixel(x, y, r, g, b);
					p += dx;
					if (p * 2 >= dy) {
						x++;
						p -= dy;
					}
				}
			}
			else {
				int x = x1;
				int p = 0;
		
				for (int y = y1; y <= y2; y++) {
					putPixel(x, y, r, g, b);
					p += dx;
					if (p * 2 >= dy) {
						x--;
						p -= dy;
					}
				}
			}
		}
	}

	void RenderBackendSDL::setClipArea(const Rect& cliparea) {
		SDL_Rect rect;
		rect.x = cliparea.x;
		rect.y = cliparea.y;
		rect.w = cliparea.w;
		rect.h = cliparea.h;
		SDL_SetClipRect(m_screen, &rect);
		SDL_FillRect(m_screen, &rect, 0x00);
	}
}
