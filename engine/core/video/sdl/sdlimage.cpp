/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "video/imagemanager.h"
#include "video/renderbackend.h"

#include "renderbackendsdl.h"
#include "sdlblendingfunctions.h"
#include "sdlimage.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_VIDEO);

	SDLImage::SDLImage(IResourceLoader* loader):
		Image(loader) {
		resetSdlimage();
	}

	SDLImage::SDLImage(const std::string& name, IResourceLoader* loader):
		Image(name, loader) {
		resetSdlimage();
	}

	SDLImage::SDLImage(SDL_Surface* surface):
		Image(surface) {
		resetSdlimage();
	 }

	SDLImage::SDLImage(const std::string& name, SDL_Surface* surface):
		Image(name, surface) {
		resetSdlimage();
	 }

	SDLImage::SDLImage(const uint8_t* data, uint32_t width, uint32_t height):
		Image(data, width, height) {
		resetSdlimage();
	}

	SDLImage::SDLImage(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height):
		Image(name, data, width, height) {
		resetSdlimage();
	}

	void SDLImage::resetSdlimage() {
		m_last_alpha = 255;
		m_finalized = false;
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

	void SDLImage::setSurface(SDL_Surface* surface) {
		if (m_zoom_surface) {
			SDL_FreeSurface(m_zoom_surface);
			m_zoom_surface = NULL;
		}
		reset(surface);
		resetSdlimage();
	}

	void SDL_BlitSurfaceWithAlpha( const SDL_Surface* src, const SDL_Rect* srcRect,
		SDL_Surface* dst,  SDL_Rect* dstRect, uint8_t alpha ) {
		if( 0 == alpha ) {
			return;
		}

		int32_t screenX, screenY;
		if( dstRect ) {
			screenX = dstRect->x;
			screenY = dstRect->y;
		} else {
			screenX = dst->clip_rect.x;
			screenY = dst->clip_rect.y;
		}

		int32_t width, height, tX, tY;
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
			int32_t dX = dst->clip_rect.x - screenX;
			screenX += dX;
			width -= dX;
			tX += dX;
		}

		if( ( screenX + width ) > ( dst->clip_rect.x + dst->clip_rect.w ) ) {
			int32_t dX = ( screenX + width ) - ( dst->clip_rect.x + dst->clip_rect.w );
			width -= dX;
		}

		if( screenY < dst->clip_rect.y ) {
			int32_t dY = dst->clip_rect.y - screenY;
			screenY += dY;
			height -= dY;
			tY += dY;
		}

		if( ( screenY + height ) > ( dst->clip_rect.y + dst->clip_rect.h ) ) {
			int32_t dY = ( screenY + height ) - ( dst->clip_rect.y + dst->clip_rect.h );
			height -= dY;
		}

		if( ( 0 >= height ) || ( 0 >= width ) ) {
			return;
		}

		SDL_LockSurface( dst );

		uint8_t* srcData = reinterpret_cast< uint8_t* > ( src->pixels );
		uint8_t* dstData = reinterpret_cast< uint8_t* > ( dst->pixels );

		// move data pointers to the start of the pixels we're copying
		srcData += tY * src->pitch  + tX * src->format->BytesPerPixel;
		dstData += screenY * dst->pitch + screenX * dst->format->BytesPerPixel;

		switch( src->format->BitsPerPixel ) {
			case 32: {
				switch( dst->format->BitsPerPixel ) {
					case 16: {
						if( 0xFFFF == ( dst->format->Rmask | dst->format->Gmask | dst->format->Bmask ) ) {
							for( int32_t y = height; y > 0; --y ) {
								SDL_BlendRow_RGBA8_to_RGB565( srcData, dstData, alpha, width );
								srcData += src->pitch;
								dstData += dst->pitch;
							}
						}
					}
					break;

					case 24: {
						for( int32_t y = height; y > 0; --y ) {
							SDL_BlendRow_RGBA8_to_RGB8( srcData, dstData, alpha, width );
							srcData += src->pitch;
							dstData += dst->pitch;
						}
					}
					break;

					case 32: {
						for( int32_t y = height; y > 0; --y ) {
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
						for( int32_t y = height; y > 0; --y ) {
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
		uint32_t* src_pointer = static_cast<uint32_t*>(src->pixels);
		uint32_t* src_help_pointer = src_pointer;
		uint32_t* dst_pointer = static_cast<uint32_t*>(dst->pixels);

		int32_t x, y, *sx_ca, *sy_ca;
		int32_t sx = static_cast<int32_t>(0xffff * src->w / dst->w);
		int32_t sy = static_cast<int32_t>(0xffff * src->h / dst->h);
		int32_t sx_c = 0;
		int32_t sy_c = 0;

		// Allocates memory and calculates row wide&height
		int32_t* sx_a = new int32_t[dst->w + 1];
		sx_ca = sx_a;
		for (x = 0; x <= dst->w; x++) {
			*sx_ca = sx_c;
			sx_ca++;
			sx_c &= 0xffff;
			sx_c += sx;
		}

		int32_t* sy_a = new int32_t[dst->h + 1];
		sy_ca = sy_a;
		for (y = 0; y <= dst->h; y++) {
			*sy_ca = sy_c;
			sy_ca++;
			sy_c &= 0xffff;
			sy_c += sy;
		}
		sy_ca = sy_a;

		// Transfers the image data

		if (SDL_MUSTLOCK(src)) {
			SDL_LockSurface(src);
		}
		if (SDL_MUSTLOCK(dst)) {
			SDL_LockSurface(dst);
		}

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
			src_help_pointer = (uint32_t*)((uint8_t*)src_help_pointer + (*sy_ca >> 16) * src->pitch);
		}

		if (SDL_MUSTLOCK(dst)) {
			SDL_UnlockSurface(dst);
		}
		if (SDL_MUSTLOCK(src)) {
			SDL_UnlockSurface(src);
		}

		// Free memory
		delete [] sx_a;
		delete [] sy_a;
	}

	SDL_Surface* getZoomedSurface(SDL_Surface * src, double zoomx, double zoomy) {
		if (src == NULL)
			return NULL;

		SDL_Surface *zoom_src;
		SDL_Surface *zoom_dst;
		int32_t dst_w = static_cast<int32_t>(round(src->w * zoomx));
		int32_t dst_h = static_cast<int32_t>(round(src->h * zoomy));
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

	void SDLImage::render(const Rect& rect, uint8_t alpha, uint8_t const* rgb) {
		if (alpha == 0) {
			return;
		}
		validateShared();
		SDL_Surface* target = RenderBackend::instance()->getRenderTargetSurface();
		assert(target != m_surface); // can't draw on the source surface

		if (rect.right() < 0 || rect.x > static_cast<int32_t>(target->w) ||
			rect.bottom() < 0 || rect.y > static_cast<int32_t>(target->h)) {
			return;
		}
		finalize();

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
				SDL_BlitSurface(m_surface, 0, target, &r);
			} else if (equal && m_zoom_surface) {
				SDL_BlitSurface(m_zoom_surface, 0, target, &r);
			} else {
				SDL_FreeSurface(m_zoom_surface);
				m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
				SDL_BlitSurface(m_zoom_surface, 0, target, &r);
			}
 		} else {
			if( 255 != alpha ) {
				// Special blitting routine with alpha blending:
				// dst.rgb = ( src.rgb * src.a * alpha ) + ( dst.rgb * (255 - ( src.a * alpha ) ) );
				if (!zoomed) {
					SDL_BlitSurfaceWithAlpha( m_surface, 0, target, &r, alpha );
				} else if (equal && m_zoom_surface) {
					SDL_BlitSurfaceWithAlpha(m_zoom_surface, 0, target, &r, alpha );
				} else {
					SDL_FreeSurface(m_zoom_surface);
					m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
					SDL_BlitSurfaceWithAlpha(m_zoom_surface, 0, target, &r, alpha );
				}
			} else {
				if (!zoomed) {
					SDL_BlitSurface(m_surface, 0, target, &r);
				} else if (equal && m_zoom_surface) {
					SDL_BlitSurface(m_zoom_surface, 0, target, &r);
				} else {
					SDL_FreeSurface(m_zoom_surface);
					m_zoom_surface = getZoomedSurface(m_surface, m_scale_x, m_scale_y);
					SDL_BlitSurface(m_zoom_surface, 0, target, &r);
				}
			}
		}
	}

	void SDLImage::finalize() {
		if( m_finalized ) {
			return;
		}
		// ultimate possibility to load the image
		// is used e.g. in case a cursor or gui image is freed even if there is a reference 
		if (!m_surface) {
			assert(m_state == IResource::RES_NOT_LOADED);
			load();
			assert(m_surface);
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
			bool m_isalphaoptimized = be->isAlphaOptimizerEnabled();
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

		int32_t transparent = 0;
		int32_t opaque = 0;
		int32_t semitransparent = 0;
		int32_t alphasum = 0;
		int32_t alphasquaresum = 0;
		bool colors[(1 << 12)];
		memset(colors, 0, (1 << 12) * sizeof(bool));

		int32_t bpp = src->format->BytesPerPixel;
		if(SDL_MUSTLOCK(src)) {
			SDL_LockSurface(src);
		}
		/*	In the first pass through we calculate avg(alpha), avg(alpha^2)
			and the number of semitransparent pixels.
			We also try to find a useable color.
		*/
		for(int32_t y = 0;y < src->h;y++) {
			for(int32_t x = 0;x < src->w;x++) {
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
		int32_t avgalpha = (opaque + semitransparent) ? alphasum / (opaque + semitransparent) : 0;
		int32_t alphavariance = 0;

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
		int32_t keycolor = -1;
		for(int32_t i = 0;i < (1 << 12);i++) {
			if(!colors[i]) {
				keycolor = i;
				break;
			}
		}
		if(keycolor == -1) {
			FL_DBG(_log, LMsg("sdlimage") << "Trying to alpha-optimize image. FAILED: no free color");
			return SDL_DisplayFormatAlpha(src);
		}

		SDL_Surface *dst = SDL_CreateRGBSurface((src->flags & ~(SDL_SRCALPHA)) | SDL_SWSURFACE,
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
		for(int32_t y = 0;y < dst->h;y++) {
			for(int32_t x = 0;x < dst->w;x++) {
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

	size_t SDLImage::getSize() {
		size_t zoomSize = 0;
		if (m_zoom_surface) {
			zoomSize = m_zoom_surface->h * m_zoom_surface->pitch;
		}
		if (m_surface) {
			zoomSize += m_surface->h * m_surface->pitch;
		}

		return zoomSize;
	}

	void SDLImage::useSharedImage(const ImagePtr& shared, const Rect& region) {
		if(shared->getState() != IResource::RES_LOADED) {
			shared->load();
		}
		SDL_Surface* src_surface = shared->getSurface();
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, region.w, region.h,
			src_surface->format->BitsPerPixel, src_surface->format->Rmask, src_surface->format->Gmask,
			src_surface->format->Bmask, src_surface->format->Amask);

		SDL_SetAlpha(src_surface, 0, 0);
		SDL_Rect srcrect = {
			static_cast<Sint16>(region.x),
			static_cast<Sint16>(region.y),
			static_cast<Uint16>(region.w),
			static_cast<Uint16>(region.h) };
		SDL_BlitSurface(src_surface, &srcrect, surface, NULL);
		SDL_SetAlpha(src_surface, SDL_SRCALPHA, 0);

		setSurface(surface);
		m_shared = false; // this isn't a mistake
		m_subimagerect = region;
		m_atlas_img = shared;
		m_atlas_name = shared->getName();
		setState(IResource::RES_LOADED);
	}

	void SDLImage::forceLoadInternal() {
		validateShared();
	}

	void SDLImage::validateShared() {
		if (m_atlas_name.empty()) {
			return;
		}

		if (m_atlas_img->getState() == IResource::RES_NOT_LOADED ||
			getState() == IResource::RES_NOT_LOADED) {
			load();
		}
	}

	void SDLImage::load() {
		if (!m_atlas_name.empty()) {
			// check atlas image
			// if it does not exist, it is created.
			if (!ImageManager::instance()->exists(m_atlas_name)) {
				ImagePtr newAtlas = ImageManager::instance()->create(m_atlas_name);
				m_atlas_img = newAtlas;
			}
			useSharedImage(m_atlas_img , m_subimagerect);
		} else {
			Image::load();
		}
	}

	void SDLImage::free() {
		setSurface(NULL);
		m_state = IResource::RES_NOT_LOADED;
	}
}
