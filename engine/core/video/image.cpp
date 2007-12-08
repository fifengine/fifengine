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
#include <assert.h>
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "image.h"

namespace FIFE {

	Image::Image(SDL_Surface* surface):
		m_surface(surface),
		m_xshift(0), 
		m_yshift(0) {}


	Image::~Image() {
		//assert(m_refcount == 0);
		if( m_surface ) {
			SDL_FreeSurface(m_surface);
		}
	}

	SDL_Surface* Image::getSurface() { 
		return m_surface; 
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

	void Image::render(const Rect& rect, unsigned char alpha) {
		render(rect,SDL_GetVideoSurface(),alpha);
	}

}

