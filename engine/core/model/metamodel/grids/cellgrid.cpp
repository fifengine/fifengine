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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

#include "cellgrid.h"

namespace FIFE {
	static Logger _log(LM_CELLGRID);

	CellGrid::CellGrid(bool allow_diagonals):
		FifeClass(),
		m_matrix(),
		m_inverse_matrix(),
		m_xshift(0),
		m_yshift(0),
		m_xscale(1),
		m_yscale(1),
		m_rotation(0),
		m_allow_diagonals(allow_diagonals) {
		updateMatrices();
	}

	CellGrid::~CellGrid() {
	}

	void CellGrid::getAccessibleCoordinates(const ModelCoordinate& curpos, std::vector<ModelCoordinate>& coordinates) {
		coordinates.clear();
		for (int x = curpos.x - 1; x <= curpos.x + 1; x++) {
			for (int y = curpos.y - 1; y <= curpos.y + 1; y++) {
				ModelCoordinate pt;
				pt.x = x;
				pt.y = y;
				if (isAccessible(curpos, pt)) {
					coordinates.push_back(pt);
				}
			}
		}
	}

	void CellGrid::updateMatrices() {
		m_matrix.loadRotate(m_rotation, 0.0, 0.0, 1.0);
		m_matrix.applyScale(m_xscale,m_yscale, 1);
		m_matrix.applyTranslate(m_xshift, m_yshift, 0);
		m_inverse_matrix = m_matrix.inverse();
	}

	ExactModelCoordinate CellGrid::toMapCoordinates(const ModelCoordinate& layer_coords) {
		return toMapCoordinates(intPt2doublePt(layer_coords));
	}

	int CellGrid::orientation(const ExactModelCoordinate& pt, const ExactModelCoordinate& pt1, const ExactModelCoordinate& pt2) {
		double o = (pt2.x - pt1.x) * (pt.y - pt1.y) - (pt.x - pt1.x) * (pt2.y - pt1.y);
		if (o > 0.0) {
			return 1;
		} else if (o < 0.0) {
			return -1;
		}
		return 0;
	}
	
	bool CellGrid::ptInTriangle(const ExactModelCoordinate& pt, const ExactModelCoordinate& pt1, const ExactModelCoordinate& pt2, const ExactModelCoordinate& pt3) {
		double o1 = orientation(pt1, pt2, pt);
		double o2 = orientation(pt2, pt3, pt);
		double o3 = orientation(pt3, pt1, pt);
		bool result = (o1 == o2) && (o2 == o3);
		FL_DBG(_log, LMsg("ptInTriangle, pt=") << pt << " pt1=" << pt1 << " pt2=" << pt2 << " pt3=" << pt3 << " in=" << result);
		return result;
	}
}
