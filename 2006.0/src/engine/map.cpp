/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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
#include "map.h"
#include "mapelevation.h"
#include "debugutils.h"

namespace FIFE {

	Map::Map(const std::string& mapname) : m_mapname(mapname), m_elevations(0) {}

	Map::~Map() {
		cleanup();
	}

	void Map::cleanup() {
		type_elevations::const_iterator end = m_elevations.end();
		for (type_elevations::iterator i = m_elevations.begin(); i != end; ++i) {
			delete *i;
		}

		m_elevations.clear();
	}

	const std::string& Map::getMapName() const {
		return m_mapname;
	}

	size_t Map::getElevationCount() const {
		return m_elevations.size();
	}

	MapElevation* Map::getElevation(size_t index) const {
		if (index >= getElevationCount()) {
			PANIC_PRINT("invalid elevationlevel: " << index);
		}

		return *(m_elevations.begin() + index);
	}

	void Map::addElevation(MapElevation* elevation) {
		m_elevations.push_back(elevation);
	}

}//FIFE
