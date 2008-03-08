/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/base/exception.h"
#include "util/structures/purge.h"
#include "model/metamodel/dataset.h"

#include "map.h"
#include "layer.h"

namespace FIFE {

	Map::Map(const std::string& identifier, TimeProvider* tp_master):
		AttributedClass(identifier), 
		m_timeprovider(tp_master),
		m_changelisteners(),
		m_changedlayers(),
		m_changed(false) {
	}

	Map::~Map() {
		deleteLayers();
	}

	void Map::addDataset(Dataset* dataset) {
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

	std::list<Layer*> Map::getLayers() const {
		std::list<Layer*> layers;

		std::vector<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			layers.push_back(*it);
		}

		return layers;
	}

	std::list<Layer*> Map::getLayers(const std::string& field, const std::string& value) const {
		std::list<Layer*> matches;

		std::vector<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it)->get(field) == value)
				matches.push_back(*it);
		}

		return matches;
	}

	size_t Map::getNumLayers() const {
		return m_layers.size();
	}

	Layer* Map::createLayer(const std::string& identifier, CellGrid* grid) {
		std::vector<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if(identifier == (*it)->Id())
				throw NameClash(identifier);
		}

		Layer* layer = new Layer(identifier, this, grid);
		m_layers.push_back(layer);
		m_changed = true;
		std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onLayerCreate(this, layer);
			++i;
		}
		
		return layer;
	}

	void Map::deleteLayer(Layer* layer) {
		std::vector<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it) == layer) {
				std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
				while (i != m_changelisteners.end()) {
					(*i)->onLayerDelete(this, layer);
					++i;
				}
				delete layer;
				m_layers.erase(it);
				return ;
			}
		}
		m_changed = true;
	}

	void Map::deleteLayers() {
		std::vector<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
			while (i != m_changelisteners.end()) {
				(*i)->onLayerDelete(this, *it);
				++i;
			}
		}
		purge(m_layers);
		m_layers.clear();
	}

	bool Map::update() {
		m_changedlayers.clear();
		std::vector<Layer*>::iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if ((*it)->update()) {
				m_changedlayers.push_back(*it);
			}
		}
		if (!m_changedlayers.empty()) {
			std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
			while (i != m_changelisteners.end()) {
				(*i)->onMapChanged(this, m_changedlayers);
				++i;
			}
		}
		bool retval = m_changed;
		m_changed = false;
		return retval;
	}
	
	void Map::addChangeListener(MapChangeListener* listener) {
		m_changelisteners.push_back(listener);
	}

	void Map::removeChangeListener(MapChangeListener* listener) {
		std::vector<MapChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			if ((*i) == listener) {
				m_changelisteners.erase(i);
				return;
			}
			++i;
		}
	}

} //FIFE

