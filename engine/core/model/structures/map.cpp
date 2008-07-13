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
#include <string>

// 3rd party library includes
#include <boost/lexical_cast.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/purge.h"

#include "map.h"
#include "layer.h"

namespace FIFE {

	Map::Map(const std::string& identifier, TimeProvider* tp_master):
		m_id(identifier), 
		m_timeprovider(tp_master),
		m_changelisteners(),
		m_changedlayers(),
		m_changed(false) {
	}

	Map::~Map() {
		deleteLayers();
	}

	Layer* Map::getLayer(const std::string& id) {
		std::list<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if((*it)->getId() == id)
				return *it;
		}

		throw NotFound(id);
	}

	size_t Map::getNumLayers() const {
		return m_layers.size();
	}

	Layer* Map::createLayer(const std::string& identifier, CellGrid* grid) {
		std::list<Layer*>::const_iterator it = m_layers.begin();
		for(; it != m_layers.end(); ++it) {
			if(identifier == (*it)->getId())
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
		std::list<Layer*>::iterator it = m_layers.begin();
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
		std::list<Layer*>::iterator it = m_layers.begin();
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
		std::list<Layer*>::iterator it = m_layers.begin();
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

