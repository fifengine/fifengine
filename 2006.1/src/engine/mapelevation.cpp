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

#include "mapelevation.h"
#include "maptile.h"
#include "exception.h"
#include "object.h"

#include "maptilegrid.h"
#include "mapgeometry.h"

namespace FIFE {

	MapElevation::MapElevation(const s_elevation_info& structure) 
		: m_objects() {

		setNumTileGrids( structure.numberOfTileGrids );
		for(size_t i = 0; i != structure.numberOfTileGrids; ++i) {
			setTileGrid(i, new MapTileGrid(structure.tileGrids[i]) );
		}

		setNumObjectGrids( structure.numberOfObjectGrids );
		for(size_t i = 0; i != structure.numberOfObjectGrids; ++i) {
			setObjectGrid(i, new MapGrid(structure.objectGrids[i].size,structure.objectGrids[i].geometry) );
			getObjectGrid(i)->setGridOverlay( structure.objectGrids[i].overlay );
		}

	}


	MapElevation::~MapElevation() {
		cleanup();
	}

	void MapElevation::cleanup() {
// 		delete[] m_tiles;
// 		m_tiles = 0;

		type_objects::const_iterator end = m_objects.end();
		for (type_objects::iterator i = m_objects.begin(); i != end; ++i) {
			delete *i;
		}

		m_objects.clear();
	}

	MapElevation::MapElevation(const MapElevation&) 
		: m_width(0), m_height(0),
// 		m_tiles(0),
		m_objects() {
	}

	MapElevation& MapElevation::operator=(const MapElevation&) {
		return *this;
	}

	void MapElevation::addObject(Object* obj, bool check_drawingorder) {
		if (!obj)
			return;

		if (check_drawingorder) {
            // Make sure drawing order is correct - Cheese
			int position = obj->getPosition();
	        type_objects::const_iterator end = m_objects.end();
	        for (type_objects::iterator i = m_objects.begin(); i != end; ++i) {
				if ((*i)->getPosition() > position) {
					m_objects.insert(i, obj);
					return;
				}
			}
		}
		
		m_objects.push_back(obj);
	}
	void MapElevation::addObjectFront(Object* obj) {
		if (obj)
			m_objects.push_front(obj);
	}


	const MapElevation::type_objects& MapElevation::getObjects() const {
		return m_objects;
	}

	size_t MapElevation::getNumTileGrids() const {
		return m_tilegrids.size();
	}

	void MapElevation::setNumTileGrids(size_t n) {
		m_tilegrids.reserve(n);
	}

	void MapElevation::setTileGrid(size_t idx, MapTileGrid* grid) {
		m_tilegrids[idx] = grid;
	}

	MapTileGrid* MapElevation::getTileGrid(size_t idx) {
		return m_tilegrids[idx];
	}

	size_t MapElevation::getNumObjectGrids() const {
		return m_objectgrids.size();
	}

	void MapElevation::setNumObjectGrids(size_t n) {
		m_objectgrids.reserve(n);
	}

	void MapElevation::setObjectGrid(size_t idx, MapGrid* grid) {
		m_objectgrids[idx] = grid;
	}

	MapGrid* MapElevation::getObjectGrid(size_t idx) {
		return m_objectgrids[idx];
	}

}//FIFE
