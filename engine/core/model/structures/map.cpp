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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"
#include "util/purge.h"
#include "model/metamodel/dataset.h"

#include "elevation.h"
#include "layer.h"
#include "map.h"

namespace FIFE { namespace model {

	Map::Map() 
		: AttributedClass("Map") {
	}

	Map::~Map() {
		clearElevations();
	}

	void Map::useDataset(Dataset* dataset) {
		// no duplicates
		std::vector<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			if((*it)->getTypeName() == dataset->getTypeName() && (*it)->getFilename() == dataset->getFilename()) {
				return;
			}
		}

		m_datasets.push_back(dataset);
	}

	size_t Map::getNumElevations() const {
		return m_elevations.size();
	}

	Elevation* Map::getElevation(size_t index) const {
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}

		return *(m_elevations.begin() + index);
	}

	Elevation* Map::getElevation(const std::string& name) const {
		std::vector<Elevation*>::const_iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			if((*it)->get<std::string>("Name") == name)
				return *it;
		}

		return 0;
	}

	void Map::addElevation(Elevation* elevation) {
		if( !elevation ) {
			throw NotSupported("can't add invalid elevation");
		}
		if( elevation->getMap() ) {
			throw Duplicate("elevation already in a map");
		}

		elevation->m_map = this;
		m_elevations.push_back(elevation);
	}

	void Map::insertElevation(size_t index, Elevation* elevation) {
		if( !elevation ) {
			throw NotSupported("can't add invalid elevation");
		}
		if( elevation->getMap() ) {
			throw Duplicate("elevation already in a map");
		}
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}

		m_elevations.insert(m_elevations.begin() + index,elevation);
		elevation->m_map = this;
	}

	void Map::removeElevation(size_t index) {
		if (index >= getNumElevations()) {
			throw IndexOverflow(std::string("invalid elevationlevel: ")
			                    + boost::lexical_cast<std::string>(index));
		}

		m_elevations.erase(m_elevations.begin() + index);
	}

	void Map::clearElevations() {
		purge(m_elevations);
	}

} } //FIFE::model

