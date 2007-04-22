/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "mapgrid.h"
#include "mapgeometry.h"

namespace FIFE {

	MapGrid::MapGrid(const Point& size, size_t geometry)
		: m_size(size),
		m_grid_overlay(),
 		m_shift(),
		m_geometry(GridGeometry::getGeometryFromId(geometry)) {
	}

	MapGrid::~MapGrid() {
	}

	const Point& MapGrid::getSize() const {
		return m_size;
	}

	GridGeometry* MapGrid::getGeometry() {
		return m_geometry;
	}

	void MapGrid::setShift(const Point& shift) {
		m_shift = shift;
	}

	const Point& MapGrid::getShift() const {
		return m_shift;
	}

	void MapGrid::setGridOverlay(const std::string& id) {
		m_grid_overlay = id;
	}

	const std::string& MapGrid::getGridOverlay() const {
		return m_grid_overlay;
	}

}
