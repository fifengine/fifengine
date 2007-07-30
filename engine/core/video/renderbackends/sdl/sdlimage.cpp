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
#include <cassert>
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/rect.h"

#include "sdlblendingfunctions.h"
#include "sdlimage.h"

namespace FIFE {

	SDLImage::SDLImage(SDL_Surface* surface): 
		Image(surface),
		m_last_alpha(255) {
		if (m_surface->format->Amask == 0) {
			SDL_SetAlpha(m_surface, SDL_SRCALPHA | SDL_RLEACCEL, 255);
			m_surface = SDL_DisplayFormat(m_surface);
		} else {
			SDL_SetAlpha(m_surface, SDL_SRCALPHA, 255);
			m_surface = SDL_DisplayFormatAlpha(m_surface);
		}
	 }

	SDLImage::~SDLImage() { }


	void SDL_BlitSurfaceWithAlpha( const SDL_Surface* src, const SDL_Rect* srcRect,
	                               SDL_Surface* dst,  SDL_Rect* dstRect, unsigned char alpha ) {
		if( 0 == alpha ) {
			return;
		}

		int screenX, screenY;
		if( dstRect ) {
			screenX = dstRect->x;
			screenY = dstRect->y;
		} else {
			screenX = dst->clip_rect.x;
			screenY = dst->clip_rect.y;
		}

		int width, height, tX, tY;
		if( srcRect ) {
			tX = srcRect->x;
			tY = srcRect->y;
			width = srcRect->w;
			height = srcRect->h;
		} else {
			tX = src->clip_rect.x;
			tY = src->clip_rect.y;
			width = src->clip_rect.w;
			height = src->clip_rect.h;
		}

		// Clipping.
		if( ( screenX >= ( dst->clip_rect.x + dst->clip_rect.w ) ) ||
			( screenY >= ( dst->clip_rect.y + dst->clip_rect.h ) ) ||
			( ( screenX + width ) <= dst->clip_rect.x ) ||
			( ( screenY + height ) <= dst->clip_rect.y ) ) {
			return;
		}

		if( screenX < dst->clip_rect.x ) {
			int dX = dst->clip_rect.x - screenX;
			screenX += dX;
			width -= dX;
			tX += dX;
		}

		if( ( screenX + width ) > ( dst->clip_rect.x + dst->clip_rect.w ) ) {
			int dX = ( screenX + width ) - ( dst->clip_rect.x + dst->clip_rect.w );
			width -= dX;
		}

		if( screenY < dst->clip_rect.y ) {
			int dY = dst->clip_rect.y - screenY;
			screenY += dY;
			height -= dY;
			tY += dY;
		}

		if( ( screenY + height ) > ( dst->clip_rect.y + dst->clip_rect.h ) ) {
			int dY = ( screenY + height ) - ( dst->clip_rect.y + dst->clip_rect.h );
			height -= dY;
		}

		if( ( 0 >= height ) || ( 0 >= width ) ) {
			return;
		}

		SDL_LockSurface( dst );

		unsigned char* srcData = reinterpret_cast< unsigned char* > ( src->pixels );
		unsigned char* dstData = reinterpret_cast< unsigned char* > ( dst->pixels );
		
		// move data pointers to the start of the pixels we're copying
		srcData += tY * src->pitch  + tX * src->format->BytesPerPixel;
		dstData += screenY * dst->pitch + screenX * dst->format->BytesPerPixel;

		switch( src->format->BitsPerPixel ) {
			case 32: {
				switch( dst->format->BitsPerPixel ) {
					case 16: {
						if( 0xFFFF == ( dst->format->Rmask | dst->format->Gmask | dst->format->Bmask ) ) {
							for( int y = height; y > 0; --y ) {
								SDL_BlendRow_RGBA8_to_RGB565( srcData, dstData, alpha, width );
								srcData += src->pitch;
								dstData += dst->pitch;
							}
						}
					}
					break;

					case 24: {
						for( int y = height; y > 0; --y ) {
							SDL_BlendRow_RGBA8_to_RGB8( srcData, dstData, alpha, width );
							srcData += src->pitch;
							dstData += dst->pitch;
						}
					}
					break;

					case 32: {
						for( int y = height; y > 0; --y ) {
							SDL_BlendRow_RGBA8_to_RGBA8( srcData, dstData, alpha, width );
							srcData += src->pitch;
							dstData += dst->pitch;
						}
					}
					break;

					default:
						break;
				}	///< switch( dst->format->BitsPerPixel )
			}
			break;

			case 16: {
				if( 0x000F == src->format->Amask ) {
					if( ( 16 == dst->format->BitsPerPixel ) &&
						( 0xFFFF == ( dst->format->Rmask | dst->format->Gmask | dst->format->Bmask ) ) ) {
						for( int y = height; y > 0; --y ) {
							SDL_BlendRow_RGBA4_to_RGB565( srcData, dstData, alpha, width );
							srcData += src->pitch;
							dstData += dst->pitch;
						}
					}
				}
			}
			break;

			default:
				break;
		}	///< switch( src->format->BitsPerPixel )

		SDL_UnlockSurface( dst );
	}

	void SDLImage::render(const Rect& rect, SDL_Surface* screen, unsigned char alpha) {
		if (alpha == 0) {
			return;
		}

		if (rect.right() < 0 || rect.x > static_cast<int>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->h)) {
			return;
		}

		SDL_Surface* surface = screen;
		SDL_Rect r;
		r.x = rect.x;
		r.y = rect.y;
		r.w = rect.w;
		r.h = rect.h;
		
		if (m_surface->format->Amask == 0) {
			// Image has no alpha channel. This allows us to use the per-surface alpha.
			if (m_last_alpha != alpha) {
				m_last_alpha = alpha;
				SDL_SetAlpha(m_surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
			}
			SDL_BlitSurface(m_surface, 0, surface, &r);
		} else {
			if( 255 != alpha ) {
				// Special blitting routine with alpha blending:
				// dst.rgb = ( src.rgb * src.a * alpha ) + ( dst.rgb * (255 - ( src.a * alpha ) ) );
				SDL_BlitSurfaceWithAlpha( m_surface, 0, surface, &r, alpha );
			} else {
				SDL_BlitSurface(m_surface, 0, surface, &r);
			}
		}
	}

	unsigned int SDLImage::getWidth() const {
		return m_surface->w;
	}

	unsigned int SDLImage::getHeight() const {
		return m_surface->h;
	}

}
