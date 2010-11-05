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
#include <cassert>
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/structures/rect.h"
#include "video/renderbackend.h"

#include "renderbackendsdl.h"
#include "sdlblendingfunctions.h"
#include "sdlimage.h"

namespace FIFE {
	static Logger _log(LM_VIDEO);

	SDLImage::SDLImage(SDL_Surface* surface):
		Image(surface) {
		resetSdlimage();
	 }

	SDLImage::SDLImage(const uint8_t* data, unsigned int width, unsigned int height):
		Image(data, width, height) {
		resetSdlimage();
	}

	void SDLImage::resetSdlimage() {
		m_last_alpha = 255;
		m_finalized = false;
		m_isalphaoptimized = false;
		m_colorkey = RenderBackend::instance()->getColorKey();
		m_scale_x = 1.0;
		m_scale_y = 1.0;
		m_zoom_surface = NULL;
	}

	SDLImage::~SDLImage() {
		if (m_zoom_surface) {
			SDL_FreeSurface(m_zoom_surface);
		}
	}

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

	void zoomSurface(SDL_Surface* src, SDL_Surface* dst) {
		SDL_Color* src_pointer = (SDL_Color*)src->pixels;
		SDL_Color* src_help_pointer = src_pointer;
		SDL_Color* dst_pointer = (SDL_Color*)dst->pixels;

		int x, y, *sx_ca, *sy_ca;
		int dst_gap = dst->pitch - dst->w * dst->format->BytesPerPixel;
		int sx = static_cast<int>(0xffff * src->w / dst->w);
		int sy = static_cast<int>(0xffff * src->h / dst->h);
		int sx_c = 0;
		int sy_c = 0;

		// Allocates memory and calculates row wide&height
		int* sx_a = (int*)malloc((dst->w + 1) * sizeof(Uint32));
		if (sx_a == NULL) {
			return;
		} else {
			sx_ca = sx_a;
			for (x = 0; x <= dst->w; x++) {
				*sx_ca = sx_c;
				sx_ca++;
				sx_c &= 0xffff;
				sx_c += sx;
			}
		}
		int* sy_a = (int*)malloc((dst->h + 1) * sizeof(Uint32));
		if (sy_a == NULL) {
			free(sx_a);
			return;
		} else {
			sy_ca = sy_a;
			for (y = 0; y <= dst->h; y++) {
				*sy_ca = sy_c;
				sy_ca++;
				sy_c &= 0xffff;
				sy_c += sy;
			}
			sy_ca = sy_a;
		}

		// Transfers the image data

		if(SDL_MUSTLOCK(src))
			SDL_LockSurface(src);
		if(SDL_MUSTLOCK(dst))
			SDL_LockSurface(dst);

		for (y = 0; y < dst->h; y++) {
			src_pointer = src_help_pointer;
			sx_ca = sx_a;
			for (x = 0; x < dst->w; x++) {
				*dst_pointer = *src_pointer;
				sx_ca++;
				src_pointer += (*sx_ca >> 16);
				dst_pointer++;
			}
			sy_ca++;
			src_help_pointer = (SDL_Color*)((Uint8*)src_help_pointer + (*sy_ca >> 16) * src->pitch);
			dst_pointer = (SDL_Color*)((Uint8*)dst_pointer + dst_gap);
		}

		if(SDL_MUSTLOCK(dst))
			SDL_UnlockSurface(dst);
		if(SDL_MUSTLOCK(src))
			SDL_UnlockSurface(src);

		// Free memory
		free(sx_a);
		free(sy_a);
	}

	SDL_Surface* getZoomedSurface(SDL_Surface * src, double zoomx, double zoomy) {
		if (src == NULL)
			return NULL;

		SDL_Surface *zoom_src;
		SDL_Surface *zoom_dst;
		int dst_w = static_cast<int>(round(src->w * zoomx));
		int dst_h = static_cast<int>(round(src->h * zoomy));
		if (dst_w < 1)
			dst_w = 1;
		if (dst_h < 1)
			dst_h = 1;

		// If source surface has no alpha channel then convert it
		if (src->format->Amask == 0) {
			zoom_src = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, 32,
					RMASK, GMASK,
					BMASK, AMASK);
			SDL_BlitSurface(src, NULL, zoom_src, NULL);
		} else {
			zoom_src = src;
		}
		// Create destination surface
		zoom_dst = SDL_CreateRGBSurface(SDL_SWSURFACE, dst_w, dst_h, 32,
				zoom_src->format->Rmask, zoom_src->format->Gmask,
				zoom_src->format->Bmask, zoom_src->format->Amask);

		// Zoom surface
		zoomSurface(zoom_src, zoom_dst);

		return zoom_dst;
	}

	bool nearlyEqual(float a, float b) {
		return ABS(a - b) <= 0.00001;
	}

	void SDLImage::render(const Rect& rect, SDL_Surface* screen, unsigned char alpha) {
		if (alpha == 0) {
			return;
		}

		if (rect.right() < 0 || rect.x > static_cast<int>(screen->w) || rect.bottom() < 0 || rect.y > static_cast<int>(screen->h)) {
			return;
		}
		finalize();

		SDL_Surface* surface = screen;
		SDL_Rect r;
		r.x = rect.x;
		r.y = rect.y;
		r.w = rect.w;
		r.h = rect.h;

		float scale_x = static_cast<float>(rect.w) / static_cast<float>(m_surface->w);
		float scale_y = static_cast<float>(rect.h) / static_cast<float>(m_surface->h);
		bool zoomed = false;
		bool equal = false;

		if (!nearlyEqual(scale_x, 1.0) && !nearlyEqual(scale_y, 1.0)) {
			zoomed = true;
			if(nearlyEqual(m_scale_x, scale_x) && nearlyEqual(m_scale_y, scale_y)) {
				equal = true;
			} else {
				m_scale_x = scale_x;
				m_scale_y = scale_y;
			}
		}

		if (m_surface->format->Amask == 0) {
			// Image has no alpha channel. This allows us to use the per-surface alpha.
			if (m_last_alpha != alpha) {
				m_last_alpha = alpha;
				SDL_SetAlpha(m_surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha);
			}
			if (!zoomed) {
				SDL_BlitSurface(m_surface, 0, surface, &r);
			} else if (equal && m_zoom_surface) {
				SDL_BlitSurface(m_zoom_surface, 0, surface, &r);
			} else {
				SDL_FreeSurface(m_zoom_surface);
				m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
				SDL_BlitSurface(m_zoom_surface, 0, surface, &r);
			}
		} else {
			if( 255 != alpha ) {
				// Special blitting routine with alpha blending:
				// dst.rgb = ( src.rgb * src.a * alpha ) + ( dst.rgb * (255 - ( src.a * alpha ) ) );
				if (!zoomed) {
					SDL_BlitSurfaceWithAlpha( m_surface, 0, surface, &r, alpha );
				} else if (equal && m_zoom_surface) {
					SDL_BlitSurfaceWithAlpha(m_zoom_surface, 0, surface, &r, alpha );
				} else {
					SDL_FreeSurface(m_zoom_surface);
					m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
					SDL_BlitSurfaceWithAlpha(m_zoom_surface, 0, surface, &r, alpha );
				}
			} else {
				if (!zoomed) {
					SDL_BlitSurface(m_surface, 0, surface, &r);
				} else if (equal && m_zoom_surface) {
					SDL_BlitSurface(m_zoom_surface, 0, surface, &r);
				} else {
					SDL_FreeSurface(m_zoom_surface);
					m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
					SDL_BlitSurface(m_zoom_surface, 0, surface, &r);
				}
			}
		}
	}

	void SDLImage::finalize() {
		if( m_finalized ) {
			return;
		}
		m_finalized = true;
		SDL_Surface *old_surface = m_surface;
		Uint32 key = SDL_MapRGB(m_surface->format, m_colorkey.r, m_colorkey.g, m_colorkey.b);

		if (m_surface->format->Amask == 0) {
			// only use color key if feature is enabled
			if (RenderBackend::instance()->isColorKeyEnabled()) {
				SDL_SetColorKey(m_surface, SDL_SRCCOLORKEY, key);
			}

			m_surface = SDL_DisplayFormat(m_surface);
		} else {
			RenderBackendSDL* be = static_cast<RenderBackendSDL*>(RenderBackend::instance());
			m_isalphaoptimized = be->isAlphaOptimizerEnabled();
			if( m_isalphaoptimized ) {
				m_surface = optimize(m_surface);
			} else  {
				SDL_SetAlpha(m_surface, SDL_SRCALPHA, 255);

				// only use color key if feature is enabled
				if (RenderBackend::instance()->isColorKeyEnabled()) {
					SDL_SetColorKey(m_surface, SDL_SRCCOLORKEY, key);
				}

				m_surface = SDL_DisplayFormatAlpha(m_surface);
			}
		}
		SDL_FreeSurface(old_surface);
	}

	SDL_Surface* SDLImage::optimize(SDL_Surface* src) {
		// The algorithm is originally by "Tim Goya" <tuxdev103@gmail.com>
		// Few modifications and adaptions by the FIFE team.
		//
		// It tries to determine whether an image with a alpha channel
		// actually uses that. Often PNGs contains an alpha channels
		// as they don't provide a colorkey feature(?) - so to speed
		// up SDL rendering we try to remove the alpha channel.

		// As a reminder: src->format->Amask != 0 here

		int transparent = 0;
		int opaque = 0;
		int semitransparent = 0;
		int alphasum = 0;
		int alphasquaresum = 0;
		bool colors[(1 << 12)];
		memset(colors, 0, (1 << 12) * sizeof(bool));

		int bpp = src->format->BytesPerPixel;
		if(SDL_MUSTLOCK(src)) {
			SDL_LockSurface(src);
		}
		/*	In the first pass through we calculate avg(alpha), avg(alpha^2)
			and the number of semitransparent pixels.
			We also try to find a useable color.
		*/
		for(int y = 0;y < src->h;y++) {
			for(int x = 0;x < src->w;x++) {
				Uint8 *pixel = (Uint8 *) src->pixels + y * src->pitch + x * bpp;
				Uint32 mapped = 0;
				switch(bpp) {
					case 1:
						mapped = *pixel;
						break;
					case 2:
						mapped = *(Uint16 *)pixel;
						break;
					case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						mapped |= pixel[0] << 16;
						mapped |= pixel[1] << 8;
						mapped |= pixel[2] << 0;
#else
						mapped |= pixel[0] << 0;
						mapped |= pixel[1] << 8;
						mapped |= pixel[2] << 16;
#endif
						break;
					case 4:
						mapped = *(Uint32 *)pixel;
						break;
				}
				Uint8 red, green, blue, alpha;
				SDL_GetRGBA(mapped, src->format, &red, &green, &blue, &alpha);
				if(alpha < 16) {
					transparent++;
				} else if (alpha > 240) {
					opaque++;
					alphasum += alpha;
					alphasquaresum += alpha*alpha;
				} else {
					semitransparent++;
					alphasum += alpha;
					alphasquaresum += alpha*alpha;
				}
				// mark the color as used.
				if( alpha != 0 ) {
					colors[((red & 0xf0) << 4) | (green & 0xf0) | ((blue & 0xf0) >> 4)] = true;
				}
			}
		}
		int avgalpha = (opaque + semitransparent) ? alphasum / (opaque + semitransparent) : 0;
		int alphavariance = 0;

		if(SDL_MUSTLOCK(src)) {
			SDL_UnlockSurface(src);
		}
		alphasquaresum /= (opaque + semitransparent) ? (opaque + semitransparent) : 1;
		alphavariance = alphasquaresum - avgalpha*avgalpha;
		if(semitransparent > ((transparent + opaque + semitransparent) / 8)
		   && alphavariance > 16) {
			FL_DBG(_log, LMsg("sdlimage")
				<< "Trying to alpha-optimize image. FAILED: real alpha usage. "
				<< " alphavariance=" << alphavariance
				<< " total=" << (transparent + opaque + semitransparent)
				<< " semitransparent=" << semitransparent
				<< "(" << (float(semitransparent)/(transparent + opaque + semitransparent))
				<< ")");
			return SDL_DisplayFormatAlpha(src);
		}

		// check availability of a suitable color as colorkey
		int keycolor = -1;
		for(int i = 0;i < (1 << 12);i++) {
			if(!colors[i]) {
				keycolor = i;
				break;
			}
		}
		if(keycolor == -1) {
			FL_DBG(_log, LMsg("sdlimage") << "Trying to alpha-optimize image. FAILED: no free color");
			return SDL_DisplayFormatAlpha(src);
		}

		SDL_Surface *dst = SDL_CreateRGBSurface(src->flags & ~(SDL_SRCALPHA) | SDL_SWSURFACE,
		                                        src->w, src->h,
		                                        src->format->BitsPerPixel,
		                                        src->format->Rmask,  src->format->Gmask,
		                                        src->format->Bmask, 0);
		bpp = dst->format->BytesPerPixel;

		Uint32 key = SDL_MapRGB(dst->format, m_colorkey.r, m_colorkey.g, m_colorkey.b);

		// if the global color key feature is disabled, then use the manually found color key
		if (!RenderBackend::instance()->isColorKeyEnabled()) {
			key = SDL_MapRGB(dst->format,
							(((keycolor & 0xf00) >> 4) | 0xf),
							((keycolor & 0xf0) | 0xf),
							(((keycolor & 0xf) << 4) | 0xf));
		}

		if(SDL_MUSTLOCK(src)) {
			SDL_LockSurface(src);
		}
		if(SDL_MUSTLOCK(dst)) {
			SDL_LockSurface(dst);
		}
		for(int y = 0;y < dst->h;y++) {
			for(int x = 0;x < dst->w;x++) {
				Uint8 *srcpixel = (Uint8 *) src->pixels + y * src->pitch + x * bpp;
				Uint8 *dstpixel = (Uint8 *) dst->pixels + y * dst->pitch + x * bpp;
				Uint32 mapped = 0;
				switch(bpp) {
					case 1:
						mapped = *srcpixel;
						break;
					case 2:
						mapped = *(Uint16 *)srcpixel;
						break;
					case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						mapped |= srcpixel[0] << 16;
						mapped |= srcpixel[1] << 8;
						mapped |= srcpixel[2] << 0;
#else
						mapped |= srcpixel[0] << 0;
						mapped |= srcpixel[1] << 8;
						mapped |= srcpixel[2] << 16;
#endif
						break;
					case 4:
						mapped = *(Uint32 *)srcpixel;
						break;
				}
				Uint8 red, green, blue, alpha;
				SDL_GetRGBA(mapped, src->format, &red, &green, &blue, &alpha);
				if(alpha < (avgalpha / 4)) {
					mapped = key;
				} else {
					mapped = SDL_MapRGB(dst->format, red, green, blue);
				}
				switch(bpp) {
					case 1:
						*dstpixel = mapped;
						break;
					case 2:
						*(Uint16 *)dstpixel = mapped;
						break;
					case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
						dstpixel[0] = (mapped >> 16) & 0xff;
						dstpixel[1] = (mapped >> 8) & 0xff;
						dstpixel[2] = (mapped >> 0) & 0xff;
#else
						dstpixel[0] = (mapped >> 0) & 0xff;
						dstpixel[1] = (mapped >> 8) & 0xff;
						dstpixel[2] = (mapped >> 16) & 0xff;
#endif
						break;
					case 4:
						*(Uint32 *)dstpixel = mapped;
						break;
				}
			}
		}
		if(SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}
		if(SDL_MUSTLOCK(src)) {
			SDL_UnlockSurface(src);
		}
		// Using the per surface alpha value does not
		// work out for mostly transparent pixels.
		// Thus disabling the part here - this needs a
		// more complex refactoring.
		// if(avgalpha < 240) {
		//	SDL_SetAlpha(dst, SDL_SRCALPHA | SDL_RLEACCEL, avgalpha);
		//}
		SDL_SetColorKey(dst, SDL_SRCCOLORKEY | SDL_RLEACCEL, key);
		SDL_Surface *convert = SDL_DisplayFormat(dst);
		SDL_FreeSurface(dst);
		FL_DBG(_log, LMsg("sdlimage ")
			<< "Trying to alpha-optimize image. SUCCESS: colorkey is " << key);
		return convert;
	} // end optimize

	bool SDLImage::putPixel(int x, int y, int r, int g, int b, int a) {
		if ((x < 0) || (x >= m_surface->w) || (y < 0) || (y >= m_surface->h)) {
			return false;
		}

		int bpp = m_surface->format->BytesPerPixel;
		SDL_LockSurface(m_surface);
		Uint8* p = (Uint8*)m_surface->pixels + y * m_surface->pitch + x * bpp;
		Uint32 pixel = SDL_MapRGB(m_surface->format, r, g, b);
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
		SDL_UnlockSurface(m_surface);
		return true;
	}

	void SDLImage::drawLine(const Point& p1, const Point& p2, int r, int g, int b, int a) {
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
					putPixel(x, y, r, g, b, a);
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
					putPixel(x, y, r, g, b, a);

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
					putPixel(x, y, r, g, b, a);
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
					putPixel(x, y, r, g, b, a);
					p += dx;
					if (p * 2 >= dy) {
						x--;
						p -= dy;
					}
				}
			}
		}
	}

	void SDLImage::drawTriangle(const Point& p1, const Point& p2, const Point& p3, int r, int g, int b, int a) {
		drawLine(p1, p2, r, g, b, a);
		drawLine(p2, p3, r, g, b, a);
		drawLine(p3, p1, r, g, b, a);
	}

	void SDLImage::drawRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		Point p1, p2, p3, p4;

		p1.x = p.x;
		p1.y = p.y;
		p2.x = p.x+w;
		p2.y = p.y;
		p3.x = p.x+w;
		p3.y = p.y+h;
		p4.x = p.x;
		p4.y = p.y+h;

		drawLine(p1, p2, r, g, b, a);
		drawLine(p2, p3, r, g, b, a);
		drawLine(p3, p4, r, g, b, a);
		drawLine(p4, p1, r, g, b, a);
	}

	void SDLImage::fillRectangle(const Point& p, uint16_t w, uint16_t h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
		SDL_Rect rect;
		rect.x = p.x;
		rect.y = p.y;
		rect.w = w;
		rect.h = h;

		Uint32 color = SDL_MapRGBA(m_surface->format, r, g, b, a);
		SDL_FillRect(m_surface, &rect, color);
	}

	void SDLImage::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4, int r, int g, int b, int a) {
		drawLine(p1, p2, r, g, b, a);
		drawLine(p2, p3, r, g, b, a);
		drawLine(p3, p4, r, g, b, a);
		drawLine(p4, p1, r, g, b, a);
	}

	void SDLImage::drawVertex(const Point& p, const uint8_t size, int r, int g, int b, int a){
		Point p1 = Point(p.x-size, p.y+size);
		Point p2 = Point(p.x+size, p.y+size);
		Point p3 = Point(p.x+size, p.y-size);
		Point p4 = Point(p.x-size, p.y-size);

		drawLine(p1, p2, r, g, b, a);
		drawLine(p2, p3, r, g, b, a);
		drawLine(p3, p4, r, g, b, a);
		drawLine(p4, p1, r, g, b, a);
	}

	void SDLImage::drawLightPrimitive(const Point& p, uint8_t intensity, float radius, int subdivisions, float xstretch, float ystretch, uint8_t red, uint8_t green, uint8_t blue) {
	}

	void SDLImage::saveImage(const std::string& filename) {
		if(m_surface) {
			const unsigned int swidth = getWidth();
			const unsigned int sheight = getHeight();
			SDL_Surface *surface = NULL;

			surface = SDL_CreateRGBSurface(SDL_SWSURFACE, swidth,
				sheight, 24,
				RMASK, GMASK, BMASK, 0);

			if(surface == NULL) {
				return;
			}

			SDL_BlitSurface(m_surface, NULL, surface, NULL);

			saveAsPng(filename, *surface);
			SDL_FreeSurface(surface);
		}
	}

	void SDLImage::setClipArea(const Rect& cliparea, bool clear) {
		SDL_Rect rect;
		rect.x = cliparea.x;
		rect.y = cliparea.y;
		rect.w = cliparea.w;
		rect.h = cliparea.h;
		SDL_SetClipRect(m_surface, &rect);
		if (clear) {
			SDL_FillRect(m_surface, &rect, 0x00);
		}
	}
}
