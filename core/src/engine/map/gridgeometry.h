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

#ifndef	FIFE_MAP_GRIDGEOMETRY_H
#define	FIFE_MAP_GRIDGEOMETRY_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "geometry.h"

namespace FIFE { namespace map {
	class GridGeometry : public Geometry {
		// An offset translation (can't be represented in the transform).
		int x_offset, y_offset;
		
		// The "tile size".
		Point m_basesize;

		// The matrix that transforms grid coordinates to screen coordinates.
		int xdx, ydx, xdy, ydy;
		// (Following the conventions of the old code)
		// [ xdx  ydx ]
		// [ xdy  ydy ]
		
		// The inverse is:
		//      1      [  ydy -ydx ]
		// determinant [ -xdy  xdx ]
		// We cache a determinant value.
		int determinant;

		public:
			GridGeometry(int x_offset, int y_offset, int width, int height,
			             int xdx, int ydx, int xdy, int ydy):
				x_offset(x_offset), y_offset(y_offset), m_basesize(width, height), 
				xdx(xdx), ydx(ydx), xdy(xdy), ydy(ydy),
				determinant(xdx * ydy - ydx * xdy) {}
		
			Point toScreen(const Point& gridPos) const;
			Point fromScreen(const Point& screenPos) const;
			// directionToGrid and getNumDirections still pure virtual.
			Point baseSize() const { return m_basesize; }
	};	
} } // namespace FIFE::map

#endif // FIFE_GRIDGEOMETRY_H

