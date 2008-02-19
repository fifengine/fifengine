/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "image.h"

namespace FIFE {

	Image::Image(SDL_Surface* surface): m_surface(NULL) {
		reset(surface);
	}

	Image::Image(const uint8_t* data, unsigned int width, unsigned int height) {
		SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, width,height, 32,
		                                            RMASK, GMASK, BMASK ,AMASK);
		SDL_LockSurface(surface);

		unsigned int size = width * height * 4;
		uint8_t* pixeldata = static_cast<uint8_t*>(surface->pixels);
		std::copy(data, data + size, pixeldata);
		SDL_UnlockSurface(surface);
		reset(surface);
	}

	void Image::reset(SDL_Surface* surface) {
		if( m_surface ) {
			SDL_FreeSurface(m_surface);
		}
		m_surface = surface;
		m_xshift = 0;
		m_yshift = 0;
		while (!m_clipstack.empty()) {
			m_clipstack.pop();
		}
		m_area.x = m_area.y = m_area.w = m_area.h = 0;
		m_surface = surface;
	}
	
	Image::~Image() {
		//assert(m_refcount == 0);
		reset(NULL);
	}

	unsigned int Image::getWidth() const {
		if (!m_surface) {
			return 0;
		}
		return m_surface->w;
	}

	unsigned int Image::getHeight() const {
		if (!m_surface) {
			return 0;
		}
		return m_surface->h;
	}
	
	const Rect& Image::getArea() {
		m_area.w = getWidth();
		m_area.h = getHeight();
		return m_area;
	}

	void Image::setXShift(int xshift) {
		m_xshift = xshift;
	}

	void Image::setYShift(int yshift) {
		m_yshift = yshift;
	}

	int Image::getXShift() const {
		return m_xshift;
	}

	int Image::getYShift() const {
		return m_yshift;
	}
	
	void Image::getPixelRGBA(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a) {
		int bpp = m_surface->format->BytesPerPixel;
		Uint8 *p = (Uint8*)m_surface->pixels + y * m_surface->pitch + x * bpp;
		uint32_t pixel;
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
		SDL_GetRGBA(pixel, m_surface->format, r, b, g, a);
	}

	void Image::drawQuad(const Point& p1, const Point& p2, const Point& p3, const Point& p4,  int r, int g, int b) {
		drawLine(p1, p2, r, g, b);
		drawLine(p2, p3, r, g, b);
		drawLine(p3, p4, r, g, b);
		drawLine(p4, p1, r, g, b);
	}

	void Image::render(const Rect& rect, unsigned char alpha) {
		render(rect, SDL_GetVideoSurface(), alpha);
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
}

