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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "cellgrid.h"

namespace FIFE {
	CellGrid::CellGrid():
		m_matrix(),
		m_inverse_matrix(),
		m_xshift(0),
		m_yshift(0),
		m_scale(1),
		m_rotation(0) {
		updateMatrices();
	}

	CellGrid::~CellGrid() {
	}

	void CellGrid::getAccessibleCoordinates(const Point& curpos, std::vector<Point>& coordinates) {
		coordinates.clear();
		for (int x = curpos.x - 1; x <= curpos.x + 1; x++) {
			for (int y = curpos.y - 1; y <= curpos.y + 1; y++) {
				Point pt;
				pt.x = x;
				pt.y = y;
				if (isAccessible(curpos, pt)) {
					coordinates.push_back(pt);
				}
			}
		}
	}

	DoublePoint CellGrid::toElevationCoords(DoublePoint layer_coords) {
		return m_matrix * layer_coords;
	}

	DoublePoint CellGrid::toLayerCoords(DoublePoint elevation_coord) {
		return m_inverse_matrix * elevation_coord;
	}

	void CellGrid::updateMatrices() {
		//m_matrix.loadRotate(M_PI/180*m_rotation, 0.0, 0.0, 1.0);
		//m_matrix.applyScale(m_scale, m_scale, 1);
		m_matrix.loadTranslate(m_xshift, m_yshift, 0);
		m_inverse_matrix = m_matrix.inverse();
	}
}
