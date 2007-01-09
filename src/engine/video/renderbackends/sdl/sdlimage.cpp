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

// Standard C++ library includes
#include <cassert>
#include <iostream>

// 3rd party library includes

// FIFE includes
#include "video/rect.h"

#include "sdlblendingfunctions.h"
#include "sdlimage.h"
#include "sdlscreen.h"

namespace FIFE {

	SDLImage::SDLImage(SDL_Surface* surface) : m_surface(surface) {
		SDL_Surface* tmp = SDL_DisplayFormatAlpha(m_surface);
		SDL_FreeSurface(m_surface);
		m_surface = tmp;
	}


	SDLImage::~SDLImage() {
		SDL_FreeSurface(m_surface);
	}


void SDL_BlitSurfaceWithAlpha( const SDL_Surface* src, const SDL_Rect* srcRect,
	SDL_Surface* dst,  SDL_Rect* dstRect, unsigned char alpha )	{
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

		switch( src->format->BitsPerPixel ) {
			case 32: {
				switch( dst->format->BitsPerPixel ) {
					case 16: {
						if( 0xFFFF == ( dst->format->Rmask | dst->format->Gmask | dst->format->Bmask ) ) {
							for( int y = 0; y < height; ++y ) {
								int srcOffset = ( y + tY ) * src->pitch + ( tX * 4 );
								int dstOffset = ( y + screenY ) * dst->pitch + ( screenX * 2 );

								SDL_BlendRow_RGBA8_to_RGB565( reinterpret_cast< unsigned char* >( src->pixels ) + srcOffset,
									reinterpret_cast< unsigned char* > ( dst->pixels ) + dstOffset, alpha, width );
							}
						}
					}
					break;

					case 24: {
						for( int y = 0; y < height; ++y ) {
							int srcOffset = ( y + tY ) * src->pitch + ( tX * 4 );
							int dstOffset = ( y + screenY ) * dst->pitch + ( screenX * 3 );

							SDL_BlendRow_RGBA8_to_RGB8( reinterpret_cast< unsigned char* >( src->pixels ) + srcOffset,
								reinterpret_cast< unsigned char* >( dst->pixels ) + dstOffset, alpha, width );
						}
					}
					break;

					case 32: {
						for( int y = 0; y < height; ++y ) {
							int srcOffset = ( y + tY ) * src->pitch + ( tX * 4 );
							int dstOffset = ( y + screenY ) * dst->pitch + ( screenX * 4 );

							SDL_BlendRow_RGBA8_to_RGBA8( reinterpret_cast< unsigned char* >( src->pixels ) + srcOffset,
								reinterpret_cast< unsigned char* >( dst->pixels ) + dstOffset, alpha, width );
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
						for( int y = 0; y < height; ++y ) {
							int srcOffset = ( y + tY ) * src->pitch + ( tX * 2 );
							int dstOffset = ( y + screenY ) * dst->pitch + ( screenX * 2 );

							SDL_BlendRow_RGBA4_to_RGB565( reinterpret_cast< unsigned char* >( src->pixels ) + srcOffset,
								reinterpret_cast< unsigned char* > ( dst->pixels ) + dstOffset, alpha, width );
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

	void SDLImage::render(const Rect& rect, Screen* screen, unsigned char alpha) {
		SDLScreen* sdlscreen = dynamic_cast<SDLScreen*>(screen);
		assert(sdlscreen);

		if (rect.right() < 0 || rect.x > static_cast<int>(screen->getWidth()) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->getHeight())) {
			return;
		}

		SDL_Surface* surface = sdlscreen->getSurface();
		SDL_Rect r;
		r.x = rect.x;
		r.y = rect.y;

		// Setting transparency.
		if( 255 != alpha ) {
			// Special blitting routine with alpha blending:
			// dst.rgb = ( src.rgb * src.a * alpha ) + ( dst.rgb * (255 - ( src.a * alpha ) ) );
			SDL_BlitSurfaceWithAlpha( m_surface, 0, surface, &r, alpha );
		} else {
			SDL_BlitSurface(m_surface, 0, surface, &r);
		}
	}

	unsigned int SDLImage::getWidth() const {
		return m_surface->w;
	}

	unsigned int SDLImage::getHeight() const {
		return m_surface->h;
	}

}
