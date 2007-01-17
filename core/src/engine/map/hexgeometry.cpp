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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "debugutils.h"

#include "hexgeometry.h"

namespace FIFE { namespace map {
	const int grid_dx_values[][6] = {
		{ -1, -1,  0,  1,  1,  0 },
		{ -1, -1,  0,  1,  1,  0 },
	};

	const int grid_dy_values[][6] = {
		{  0,  1,  1,  1,  0, -1 },
		{ -1,  0,  1,  0, -1, -1 }
	};

	const int sgrid_mul_values[][6] = {
		{  1,  2,  1, -1,  -2, -1 },
		{ -1,  0,  1,  1,   0, -1 }
	};


	HexGeometry::HexGeometry(const s_geometry_info& g, const Point& mapsize) {
		m_basesize = g.size;

		m_offset -= toScreen(Point(mapsize.x-1,0)) - g.size;
		Log(g.geometry)
			<< "screenBoundingRect("<<Rect(0,0,mapsize.x,mapsize.y)<<") -> " 
			<< screenBoundingRect(Rect(0,0,mapsize.x,mapsize.y));
	}

	size_t HexGeometry::getNumDirections() const {
		return 6;
	}

	Point HexGeometry::toScreen(const Point& pos) const {
		int32_t w  = m_basesize.x;
		int32_t h  = m_basesize.y;
		return Point(m_offset.x - (pos.x*w - (pos.x/2)*(w/2) - pos.y * h),
                         m_offset.y + (pos.x/2)*(3*h/4) + pos.y*(3*h/4));
	}
	
	Point HexGeometry::fromScreen(const Point& pos) const {
		int32_t w  = m_basesize.x;
		int32_t h  = m_basesize.y;

		Point p2((pos.x - m_offset.x)/(-w/2), (pos.y - m_offset.y)/(3*h/4));
		p2.x = (p2.x + p2.y)/2;
		p2.y = p2.y - p2.x/2;
		return p2;
	}

	Point HexGeometry::directionToGrid(size_t direction, const Point& at) const {
		assert(direction >= 0 && direction < getNumDirections());
		int parity = at.x & 1;
		return Point(grid_dx_values[parity][direction],
		             grid_dy_values[parity][direction]);
	}

	Point HexGeometry::directionToScreen(size_t dir) const {
		assert(dir < getNumDirections());
		return Point(sgrid_mul_values[0][dir] * m_basesize.x/2,
		             sgrid_mul_values[1][dir] * m_basesize.y);
	}


} } // FIFE::map
