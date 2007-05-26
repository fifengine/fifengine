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
#include "util/debugutils.h"

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

	const float hex_adjacent_costs[6] = { 1,1,1,1,1,1 };


	HexGeometry::HexGeometry(const s_geometry_info& g, const Point& mapsize) {
		m_basesize = g.size;
		Point delta;
		m_transform = g.transform;

		if( g.flags & Geometry::ShiftXAxis) {
			delta -= toScreen(Point(mapsize.x-1,0));
		}

		if( g.flags & 2) {
			delta -= toScreen(Point(0,mapsize.y-1));
		}

		m_offset += delta + g.offset;
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
		int32_t dx  = m_transform.x;
		int32_t dy  = m_transform.y;
		return Point(m_offset.x - (pos.x*w - (pos.x/2)*dx - pos.y * h),
		             m_offset.y + (pos.x/2)*dy + pos.y*dy);
	}
	
	Point HexGeometry::fromScreen(const Point& pos) const {

		// tile box (for hit testing) 
		float dx = 2 * m_transform.x;
		float dy = m_transform.y;

		// not sure where this offset is coming from; it may just be
		// there to align us with the relative origin
		float x = static_cast<float>(pos.x - m_offset.x);
		float y = static_cast<float>(pos.y - m_offset.y);

		// the great divide; convert screen coordinates to tile coordinates;
		// coordinates should now be considered on the skewed axes; note that
		// the x coordinates are reversed. This divides the space into a grid
		// we will hit-test within a box on this grid.
		Point mouse_box = Point(static_cast<int>(ceil(x/(-dx))),
		                        static_cast<int>(round(y/dy)));

		Point hex = Point(0,0);

		// because of the skewed axes, the hex x and y coordinates are both
		// dependent on both the mouse x and y coordinates; this logic
		// describes the relationship. A picture and further comments can
		// be found on the wiki at:
		// http://wiki.fifengine.de/index.php?title=Fallout%27s_map_geometry#Hexagonal_Implementation_Details
		if(mouse_box.y % 2 == 0) {
			// recompute mousebox.x and round since boxes don't line up on even rows
			hex.x = mouse_box.y/2 + round(x/(-dx));
		}
		else {
			hex.x = (mouse_box.y - 1)/2 + mouse_box.x;
		}

		if(hex.x % 2 == 0) {
			hex.y = mouse_box.y - hex.x/2 - 1;
		}
		else {
			hex.y = mouse_box.y - (hex.x + 1)/2;
		}

		// FIXME: hit testing is not pixel-perfect; hexs are just being
		// approximated by boxes. Even accounting for that, things still
		// seem to be a bit off. Still, this seems much better than before.
		// -jwt

		return hex;
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
		             sgrid_mul_values[1][dir] * m_transform.y);
	}

	const float* HexGeometry::getAdjacentCosts() const {
		return hex_adjacent_costs;
	}

} } // FIFE::map
