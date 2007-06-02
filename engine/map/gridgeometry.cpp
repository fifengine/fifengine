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

// Platform specific includes
#include "util/fife_math.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log.h"

#include "gridgeometry.h"

namespace FIFE { namespace map {
	const int grid_dx_values[] = {
		 0,   1,  1,  1, 0, -1, -1, -1 
	};

	const int grid_dy_values[] = {
		-1,  -1,  0,  1, 1,  1,  0, -1
	};

	const float grid_adjacent_costs[8] = { 1,M_SQRT2, 1,M_SQRT2, 1,M_SQRT2, 1,M_SQRT2 };

	size_t GridGeometry::getNumDirections() const {
		return 8;
	}

	GridGeometry::GridGeometry(const s_geometry_info& g, const Point& mapsize) {
		m_basesize = g.size;
		m_offset   = g.offset;
		
		xdx = g.transform.x;
		ydy = g.transform.y;
		xdy = ydy - m_basesize.y;
		ydx = m_basesize.x - xdx;
		determinant = xdx*ydy - xdy*ydx;
		assert(determinant);

		Log(g.geometry)
			<< "[ " << xdx << ", " << ydx << "]"
			<< "[ " << xdy << ", " << ydy << "]: "
			<< "screenBoundingRect("<<Rect(0,0,mapsize.x,mapsize.y)<<") -> " 
			<< screenBoundingRect(Rect(0,0,mapsize.x,mapsize.y));
	}

	Point GridGeometry::directionToGrid(size_t direction, const Point&) const {
		assert(direction >= 0 && direction < getNumDirections());
		return Point(grid_dx_values[direction],
		             grid_dy_values[direction]);
	}

	Point GridGeometry::toScreen(const Point& pos) const {
		return Point(pos.x * xdx + pos.y * ydx + m_offset.x,
		             pos.x * xdy + pos.y * ydy + m_offset.y);
	}

	Point GridGeometry::fromScreen(const Point& pos) const {
		Point p2(pos.x - m_offset.x, pos.y - m_offset.y);

		float x = static_cast<float>(p2.x * ydy + p2.y * -ydx) / determinant;
		float y = static_cast<float>(p2.x * -xdy + p2.y * xdx) / determinant;

		// FIXME: There's an off-by-one error here; I don't know what's
		// introducing it, although perhaps its the dx/dy values; there's 
		// a similar problem for hexs. -jwt
		return Point(static_cast<int>(round(x)),
		             static_cast<int>(round(y)) - 1);
	} 

	const float* GridGeometry::getAdjacentCosts() const {
		return grid_adjacent_costs;
	}

} } // FIFE::map

