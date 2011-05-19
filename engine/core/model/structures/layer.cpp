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

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/structures/purge.h"

#include "layer.h"
#include "instance.h"
#include "map.h"
#include "instancetree.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	Layer::Layer(const std::string& identifier, Map* map, CellGrid* grid)
		: m_id(identifier),
		m_map(map),
		m_instances_visibility(true),
		m_transparency(0),
		m_instanceTree(new InstanceTree()),
		m_grid(grid),
		m_pathingstrategy(CELL_EDGES_ONLY),
		m_changelisteners(),
		m_changedinstances(),
		m_changed(false) {
	}

	Layer::~Layer() {
		purge(m_instances);
		delete m_instanceTree;
	}

	bool Layer::hasInstances() const {
		return !m_instances.empty();
	}

	Instance* Layer::createInstance(Object* object, const ModelCoordinate& p, const std::string& id) {
		ExactModelCoordinate emc(static_cast<double>(p.x), static_cast<double>(p.y), static_cast<double>(p.z));
		return createInstance(object, emc, id);
	}

	Instance* Layer::createInstance(Object* object, const ExactModelCoordinate& p, const std::string& id) {
		Location location;
		location.setLayer(this);
		location.setExactLayerCoordinates(p);

		Instance* instance = new Instance(object, location, id);
		if(instance->isActive()) {
			setInstanceActivityStatus(instance, instance->isActive());
		}
		m_instances.push_back(instance);
		m_instanceTree->addInstance(instance);

		std::vector<LayerChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onInstanceCreate(this, instance);
			++i;
		}
		m_changed = true;
		return instance;
	}

	bool Layer::addInstance(Instance* instance, const ExactModelCoordinate& p){
        if( !instance ){
            FL_ERR(_log, "Tried to add an instance to layer, but given instance is invalid");
            return false;
        }

	    Location location;
		location.setLayer(this);
		location.setExactLayerCoordinates(p);
		instance->setLocation(location);

		m_instances.push_back(instance);
		m_instanceTree->addInstance(instance);
		if(instance->isActive()) {
			setInstanceActivityStatus(instance, instance->isActive());
		}

		std::vector<LayerChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onInstanceCreate(this, instance);
			++i;
		}
		m_changed = true;
		return true;
	}

	void Layer::deleteInstance(Instance* instance) {
		std::vector<LayerChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			(*i)->onInstanceDelete(this, instance);
			++i;
		}
		setInstanceActivityStatus(instance, false);
		std::vector<Instance*>::iterator it = m_instances.begin();
		for(; it != m_instances.end(); ++it) {
			if(*it == instance) {
				m_instanceTree->removeInstance(*it);
				delete *it;
				m_instances.erase(it);
				break;
			}
		}
		m_changed = true;
	}

	void Layer::setInstanceActivityStatus(Instance* instance, bool active) {
		if(active) {
			m_active_instances.insert(instance);
		} else {
			m_active_instances.erase(instance);
		}
	}

	Instance* Layer::getInstance(const std::string& id) {
		std::vector<Instance*>::iterator it = m_instances.begin();
		for(; it != m_instances.end(); ++it) {
			if((*it)->getId() == id)
				return *it;
		}

		return 0;
	}

	std::vector<Instance*> Layer::getInstances(const std::string& id) {
		std::vector<Instance*> matching_instances;
		std::vector<Instance*>::iterator it = m_instances.begin();
		for(; it != m_instances.end(); ++it) {
			if((*it)->getId() == id)
				matching_instances.push_back(*it);
		}
		return matching_instances;
	}

	std::vector<Instance*> Layer::getInstancesAt(Location& loc, bool use_exactcoordinates) {
		std::vector<Instance*> matching_instances;
		std::vector<Instance*>::iterator it = m_instances.begin();

		for(; it != m_instances.end(); ++it) {
			if (use_exactcoordinates) {
				if ((*it)->getLocationRef().getExactLayerCoordinatesRef() == loc.getExactLayerCoordinatesRef()) {
					matching_instances.push_back(*it);
				}
			} else {
				if ((*it)->getLocationRef().getLayerCoordinates() == loc.getLayerCoordinates()) {
					matching_instances.push_back(*it);
				}
			}
		}

		return matching_instances;
	}

	void Layer::getMinMaxCoordinates(ModelCoordinate& min, ModelCoordinate& max, const Layer* layer) const {
		if (!layer) {
			layer = this;
		}

		bool first_found = false;
		for (std::vector<Instance*>::const_iterator i = m_instances.begin(); i != m_instances.end(); ++i) {
			if (!first_found) {
				min = m_instances.front()->getLocationRef().getLayerCoordinates(layer);
				max = min;
				first_found = true;
			} else {
				ModelCoordinate coord = (*i)->getLocationRef().getLayerCoordinates(layer);

				if(coord.x < min.x) {
					min.x = coord.x;
				}

				if(coord.x > max.x) {
					max.x = coord.x;
				}

				if(coord.y < min.y) {
					min.y = coord.y;
				}

				if(coord.y > max.y) {
					max.y = coord.y;
				}
			}
		}
		if (!first_found) {
			min = ModelCoordinate();
			max = min;
		}
	}

	void Layer::setInstancesVisible(bool vis) {
		m_instances_visibility = vis;
	}

	void Layer::setLayerTransparency(uint8_t transparency) {
		m_transparency = transparency;
	}

	uint8_t Layer::getLayerTransparency() {
		return m_transparency;
	}

	void Layer::toggleInstancesVisible() {
		m_instances_visibility = !m_instances_visibility;
	}

	bool Layer::cellContainsBlockingInstance(const ModelCoordinate& cellCoordinate) {
		std::list<Instance*> adjacentInstances;
		m_instanceTree->findInstances(cellCoordinate, 0, 0, adjacentInstances);
		bool blockingInstance = false;
		for(std::list<Instance*>::const_iterator j = adjacentInstances.begin(); j != adjacentInstances.end(); ++j) {
			if((*j)->isBlocking() && (*j)->getLocationRef().getLayerCoordinates() == cellCoordinate) {
				blockingInstance = true;
			}
		}
		return blockingInstance;
	}

	bool Layer::update() {
		m_changedinstances.clear();
		std::set<Instance*>::iterator it = m_active_instances.begin();
		for(; it != m_active_instances.end(); ++it) {
			if ((*it)->update() != ICHANGE_NO_CHANGES) {
				m_changedinstances.push_back(*it);
				m_changed = true;
			}
		}
		if (!m_changedinstances.empty()) {
			std::vector<LayerChangeListener*>::iterator i = m_changelisteners.begin();
			while (i != m_changelisteners.end()) {
				(*i)->onLayerChanged(this, m_changedinstances);
				++i;
			}
			//std::cout << "Layer named " << Id() << " changed = 1\n";
		}
		//std::cout << "Layer named " << Id() << " changed = 0\n";
		bool retval = m_changed;
		m_changed = false;
		return retval;
	}

	void Layer::addChangeListener(LayerChangeListener* listener) {
		m_changelisteners.push_back(listener);
	}

	void Layer::removeChangeListener(LayerChangeListener* listener) {
		std::vector<LayerChangeListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			if ((*i) == listener) {
				m_changelisteners.erase(i);
				return;
			}
			++i;
		}
	}
} // FIFE
