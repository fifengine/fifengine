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

#include "maptilegrid.h"

namespace FIFE {

	MapTileGrid::MapTileGrid(const Point& size, size_t geometry)
		: MapGrid(size,geometry),
		m_global_alpha(255),
		m_tiles(size.x*size.y) {
	}

	MapTileGrid::MapTileGrid(const s_grid_info& s)
		: MapGrid(s.size,s.geometry),
		m_global_alpha(255),
		m_tiles(s.size.x*s.size.y) {
		setShift(s.shift);
		setGridOverlay( s.overlay );
	}

	MapTileGrid::~MapTileGrid() {
	}

	size_t MapTileGrid::getImageId(const Point& p) const {
		return m_tiles[ p.x + p.y * m_size.x ];
	}

	void MapTileGrid::setImageId(const Point& p, size_t id) {
		m_tiles[ p.x + p.y * m_size.x ] = id;
	}

	uint8_t MapTileGrid::getGlobalAlpha() const {
		return m_global_alpha;
	}

	void MapTileGrid::setGlobalAlpha(uint8_t alpha) {
		m_global_alpha = alpha;
	}

	void MapTileGrid::addToGlobalAlpha(int delta) {
		m_global_alpha = std::min(255, std::max( 0, m_global_alpha + delta) );
	}
}

