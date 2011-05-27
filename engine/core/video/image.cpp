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
#include <sstream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource.h"
#include "loaders/native/video/imageloader.h"

#include "image.h"

namespace FIFE {

	Image::Image(IResourceLoader* loader):
		IResource(createUniqueImageName(), loader),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0){
	}

	Image::Image(const std::string& name, IResourceLoader* loader):
		IResource(name, loader),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0){
	}

	Image::Image(SDL_Surface* surface):
		IResource(createUniqueImageName()),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0){
		reset(surface);
	}

	Image::Image(const std::string& name, SDL_Surface* surface):
		IResource(name),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0){
		reset(surface);
	}

	//@todo make a private function to handle this
	Image::Image(const uint8_t* data, uint32_t width, uint32_t height):
		IResource(createUniqueImageName()),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0){
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width,height, 32,
		                                            RMASK, GMASK, BMASK ,AMASK);
		SDL_LockSurface(surface);

		uint32_t size = width * height * 4;
		uint8_t* pixeldata = static_cast<uint8_t*>(surface->pixels);
		std::copy(data, data + size, pixeldata);
		SDL_UnlockSurface(surface);
		reset(surface);
	}

	Image::Image(const std::string& name, const uint8_t* data, uint32_t width, uint32_t height):
		IResource(name),
		m_surface(NULL),
		m_xshift(0),
		m_yshift(0) {
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width,height, 32,
		                                            RMASK, GMASK, BMASK ,AMASK);
		SDL_LockSurface(surface);

		uint32_t size = width * height * 4;
		uint8_t* pixeldata = static_cast<uint8_t*>(surface->pixels);
		std::copy(data, data + size, pixeldata);
		SDL_UnlockSurface(surface);
		reset(surface);
	}

	void Image::reset(SDL_Surface* surface) {
		if( m_surface ) {
			SDL_FreeSurface(m_surface);
		}

		m_xshift = 0;
		m_yshift = 0;
		while (!m_clipstack.empty()) {
			m_clipstack.pop();
		}
		m_area.x = m_area.y = m_area.w = m_area.h = 0;
		m_surface = surface;
		m_isbackgroundcolor = false;
		m_backgroundcolor.r = 0;
		m_backgroundcolor.g = 0;
		m_backgroundcolor.b = 0;
	}

	Image::~Image() {
		reset(NULL);
	}

	void Image::load() {
		if (m_loader){
			m_loader->load(this);
		}
		else {
			ImageLoader loader;
			loader.load(this);
		}
		m_state = IResource::RES_LOADED;
	}

	void Image::free() {
		reset(NULL);
		m_state = IResource::RES_NOT_LOADED;
	}

	SDL_Surface* Image::detachSurface() {
		SDL_Surface* srf = m_surface;
		m_surface = NULL;
		return srf;
	}

	uint32_t Image::getWidth() const {
		if (!m_surface) {
			return 0;
		}
		return m_surface->w;
	}

	uint32_t Image::getHeight() const {
		if (!m_surface) {
			return 0;
		}
		return m_surface->h;
	}

	size_t Image::getSize() {
		if (!m_surface) {
			return 0;
		}
		return m_surface->h * m_surface->pitch;
	}

	const Rect& Image::getArea() {
		m_area.w = getWidth();
		m_area.h = getHeight();
		return m_area;
	}

	void Image::setXShift(int32_t xshift) {
		m_xshift = xshift;
	}

	void Image::setYShift(int32_t yshift) {
		m_yshift = yshift;
	}

	void Image::getPixelRGBA(int32_t x, int32_t y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		if ((x < 0) || (x >= m_surface->w) || (y < 0) || (y >= m_surface->h)) {
			r = 0;
			g = 0;
			b = 0;
			a = 0;
			return;
		}

		int32_t bpp = m_surface->format->BytesPerPixel;
		Uint8 *p = (Uint8*)m_surface->pixels + y * m_surface->pitch + x * bpp;
		uint32_t pixel = 0;
		switch(bpp) {
		case 1:
			pixel = *p;

		case 2:
			pixel = *(Uint16 *)p;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
				pixel = p[0] << 16 | p[1] << 8 | p[2];
			} else {
				pixel = p[0] | p[1] << 8 | p[2] << 16;
			}

		case 4:
			pixel = *(Uint32 *)p;
		}
		SDL_GetRGBA(pixel, m_surface->format, r, g, b, a);
	}

	void Image::render(const Rect& rect, uint8_t alpha, uint8_t const* const rgb) {
		render(rect, SDL_GetVideoSurface(), alpha, rgb);
	}

	void Image::setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
		if (r != m_backgroundcolor.r || g != m_backgroundcolor.g || b != m_backgroundcolor.b) {
			m_isbackgroundcolor = true;
			m_backgroundcolor.r = r;
			m_backgroundcolor.g = g;
			m_backgroundcolor.b = b;
		}
	}

	void Image::pushClipArea(const Rect& cliparea, bool clear) {
		ClipInfo ci;
		ci.r = cliparea;
		ci.clearing = clear;
		m_clipstack.push(ci);
		setClipArea(cliparea, clear);
	}

	void Image::popClipArea() {
		assert(!m_clipstack.empty());
		m_clipstack.pop();
		if (m_clipstack.empty()) {
			clearClipArea();
		} else {
			ClipInfo ci = m_clipstack.top();
			setClipArea(ci.r, ci.clearing);
		}
	}

	const Rect& Image::getClipArea() const {
		if (m_clipstack.empty()) {
			return m_clipstack.top().r;
		} else {
			return m_area;
		}
	}

	void Image::clearClipArea() {
		setClipArea(m_area, true);
	}

	void Image::saveAsPng(const std::string& filename, SDL_Surface& surface) {
		FILE *fp;
		png_structp pngptr;
		png_infop infoptr;
		int32_t colortype;
		png_bytep *rowpointers = NULL;

		fp = fopen(filename.c_str(), "wb");

		if (fp == NULL) {
			return;
		}

		//create the png file
		pngptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
		if (pngptr == NULL) {
			fclose(fp);
			return;
		}

		//create information struct
		infoptr = png_create_info_struct(pngptr);
		if (infoptr == NULL) {
			fclose(fp);
			png_destroy_write_struct(&pngptr, (png_infopp)NULL);
			return;
		}

		if (setjmp(png_jmpbuf(pngptr))) {
			png_destroy_write_struct(&pngptr, &infoptr);
			fclose(fp);
			return;
		}

		//initialize io
		png_init_io(pngptr, fp);

		//lock the surface for access
		SDL_LockSurface(&surface);

		colortype = PNG_COLOR_TYPE_RGB;
		if(m_surface->format->palette){
			colortype |= PNG_COLOR_TYPE_PALETTE;
		}
		else if (m_surface->format->Amask){
			colortype |= PNG_COLOR_TYPE_RGB_ALPHA;
		}
		else{}

		png_set_IHDR(pngptr, infoptr, surface.w, surface.h, 8, colortype,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

		png_write_info(pngptr, infoptr);
		png_set_packing(pngptr);

		rowpointers = new png_bytep[surface.h];
		for (int32_t i = 0; i < surface.h; i++) {
			rowpointers[i] = (png_bytep)(Uint8 *)surface.pixels + i*surface.pitch;
		}
		//write the image
		png_write_image(pngptr, rowpointers);
		png_write_end(pngptr, infoptr);

		SDL_UnlockSurface(&surface);
		delete [] rowpointers;
		png_destroy_write_struct(&pngptr, &infoptr);
		fclose(fp);

	}

	std::string Image::createUniqueImageName() {
	        // automated counting for name generation, in case the user doesn't provide a name
	        static uint32_t uniqueNumber = 0;
	        static std::string baseName = "image";

	        std::ostringstream oss;
	        oss << uniqueNumber << "_" << baseName;

	        const std::string name = oss.str();
	        ++uniqueNumber;

	        return name;
	}
}
