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

#ifndef	FIFE_MAP_HEXGEOMETRY_H
#define	FIFE_MAP_HEXGEOMETRY_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "geometry.h"

namespace FIFE { namespace map {
	/** Unskewed hexagonal geometry.
	 */
	class HexGeometry : public Geometry {
		public:
			HexGeometry(const s_geometry_info& g, const Point& mapsize);

			size_t getNumDirections() const;
			Point directionToGrid(size_t dir, const Point& at = Point()) const;
			Point directionToScreen(size_t dir) const;

			Point toScreen(const Point& gridPos) const;
			Point fromScreen(const Point& screenPos) const;
			Point baseSize() const { return m_basesize; }

			const float* getAdjacentCosts() const;
		protected:
			Point m_offset;
			Point m_transform;
			Point m_basesize;
	};
} }

#endif //  FIFE_HEXGEOMETRY_H
