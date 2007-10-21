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
#include "util/exception.h"
#include "util/purge.h"
#include "model/metamodel/dataset.h"

#include "elevation.h"
#include "layer.h"
#include "map.h"

namespace FIFE {

	Map::Map(const std::string& identifier) 
		: AttributedClass(identifier) {
	}

	Map::~Map() {
		clearElevations();
	}

	void Map::useDataset(Dataset* dataset) {
		// no duplicates
		std::vector<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			if((*it) == dataset) {
				return;
			}
		}

		m_datasets.push_back(dataset);
	}

	std::list<Dataset*> Map::getDatasets() {
		std::list<Dataset*> datasets;

		std::vector<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			datasets.push_back(*it);	
		}

		return datasets;
	}

	std::list<Dataset*> Map::getDatasetsRec() {
		std::list<Dataset*> datasets;

		std::vector<Dataset*>::iterator it = m_datasets.begin();
		for(; it != m_datasets.end(); ++it) {
			std::list<Dataset*> tmp = (*it)->getDatasetsRec();
			datasets.splice(datasets.end(), tmp);
			datasets.push_back(*it);
		}

		return datasets;
	}

	std::list<Elevation*> Map::getElevations() const {
		std::list<Elevation*> elevs;
								
		std::vector<Elevation*>::const_iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			elevs.push_back(*it);
		}
																								
		return elevs;
	}

	std::list<Elevation*> Map::getElevations(const std::string& field, const std::string& value) const {
		std::list<Elevation*> matches;

		std::vector<Elevation*>::const_iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			if((*it)->get(field) == value)
				matches.push_back(*it);
		}

		return matches;
	}


	Elevation* Map::addElevation(const std::string& identifier) {
		std::vector<Elevation*>::const_iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			if(identifier == (*it)->Id())
				throw NameClash("Elevation identifer " + identifier + " has multiple occurances in Map: " + Id() + ".");
		}

		Elevation* elevation = new Elevation(identifier, this);
		m_elevations.push_back(elevation);
		return elevation;
	}
	
	void Map::removeElevation(Elevation* elevation) {
		std::vector<Elevation*>::iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			if(*it == elevation) {
				delete *it;
				m_elevations.erase(it);
				return ;
			}
		}
	}

	size_t Map::getNumElevations() const {
		return m_elevations.size();
	}

	void Map::clearElevations() {
		purge(m_elevations);
		m_elevations.clear();
	}

	void Map::update() {
		std::vector<Elevation*>::iterator it = m_elevations.begin();
		for(; it != m_elevations.end(); ++it) {
			(*it)->update();
		}
	}

} //FIFE

