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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "debugutils.h"
#include "util/purge.h"

#include "elevation.h"
#include "layer.h"
#include "map.h"
#include "objectmanager.h"

namespace FIFE { namespace map {

	Map::Map(const std::string& mapname) 
		: AttributedClass("map"),
		m_mapname(mapname),
		m_mom(new ObjectManager()),
		m_elevations(0) {
	}

	Map::~Map() {
		cleanup();
	}

	void Map::cleanup() {
		// Delete Elevations
		m_elevations.clear();
		
		// Delete objects
		delete m_mom;
	}

	const std::string& Map::getMapName() const {
		return m_mapname;
	}
	void Map::setMapName(const std::string& name) {
		m_mapname = name;
	}

	size_t Map::getElevationCount() const {
		return m_elevations.size();
	}

	ElevationPtr Map::getElevation(size_t index) const {
		if (index >= getElevationCount()) {
			PANIC_PRINT("invalid elevationlevel: " << index);
		}

		return *(m_elevations.begin() + index);
	}

	void Map::addElevation(ElevationPtr elevation) {
		m_elevations.push_back(elevation);
	}

	void Map::setScript(Map::ScriptType st, const ScriptContainer& script) {
		m_scripts[st] = script;
	}

	bool Map::hasScript(Map::ScriptType st) const {
		type_scriptmap::const_iterator it = m_scripts.find(st);
		if (it == m_scripts.end()) {
			return false;
		}
		if (it->second.value == "") {
			return false;
		}
		return true;
	}

	const ScriptContainer& Map::getScript(Map::ScriptType st) {
		return m_scripts[st];
	}

	ObjectManager* Map::getObjectManager() {
		return m_mom;
	}

	bool Map::isValidLocation(const Location& location) const {
		if (location.elevation >= m_elevations.size())
			return false;

		if (location.layer >= m_elevations[location.elevation]->getNumLayers())
			return false;

		return m_elevations[location.elevation]->
			     getLayer(location.layer)->isValidPosition(location.position);
	}

} } //FIFE::map
