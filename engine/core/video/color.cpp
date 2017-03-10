/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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

// Platform specific includes
#include "util/base/fife_stdint.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "color.h"

namespace FIFE {
	Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
	:
	m_r(r),
	m_g(g),
	m_b(b),
	m_a(alpha) {
	}
	
	Color::~Color() {
	}
	
	void Color::set(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) {
		m_r = r;
		m_g = g;
		m_b = b;
		m_a = alpha;
	}
	
	void Color::setR(uint8_t r) {
		m_r = r;
	}
	
	void Color::setG(uint8_t g) {
		m_g = g;
	}
	
	void Color::setB(uint8_t b) {
		m_b = b;
	}
	
	void Color::setAlpha(uint8_t alpha) {
		m_a = alpha;
	}
	
	uint8_t Color::getR() const {
		return m_r;
	}
	
	uint8_t Color::getG() const {
		return m_g;
	}
	
	uint8_t Color::getB() const {
		return m_b;
	}
	
	uint8_t Color::getAlpha() const { 
		return m_a;
	}
}