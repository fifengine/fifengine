/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/structures/purge.h"
#include "model/metamodel/grids/cellgrid.h"

#include "layer.h"
#include "instance.h"
#include "map.h"
#include "instancetree.h"
#include "cell.h"
#include "cellcache.h"
#include "trigger.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_STRUCTURES);

	Layer::Layer(const std::string& identifier, Map* map, CellGrid* grid)
		: m_id(identifier),
		m_map(map),
		m_instancesVisibility(true),
		m_transparency(0),
		m_instanceTree(new InstanceTree()),
		m_grid(grid),
		m_pathingStrategy(CELL_EDGES_ONLY),
		m_sortingStrategy(SORTING_CAMERA),
		m_walkable(false),
		m_interact(false),
		m_walkableId(""),
		m_cellCache(NULL),
		m_changeListeners(),
		m_changedInstances(),
		m_changed(false),
		m_static(false) {
	}

	Layer::~Layer() {
		// if this is a walkable layer
		destroyCellCache();
		// if this is a interact layer
		if (m_interact) {
			Layer* temp = m_map->getLayer(m_walkableId);
			if (temp) {
				temp->removeInteractLayer(this);
			}
		}
		purge(m_instances);
		delete m_instanceTree;
	}

	const std::string& Layer::getId() const {
		return m_id;
	}

	void Layer::setId(const std::string& id) {
		m_id = id;
	}

	Map* Layer::getMap() const {
		return m_map;
	}

	CellGrid* Layer::getCellGrid() const {
		return m_grid;
	}

	void Layer::setCellGrid(CellGrid* grid) {
		m_grid = grid;
	}

	InstanceTree* Layer::getInstanceTree(void) const {
		return m_instanceTree;
	}

	bool Layer::hasInstances() const {
		return !m_instances.empty();
	}

	Instance* Layer::createInstance(Object* object, const ModelCoordinate& p, const std::string& id) {
		ExactModelCoordinate emc(static_cast<double>(p.x), static_cast<double>(p.y), static_cast<double>(p.z));
		return createInstance(object, emc, id);
	}

	Instance* Layer::createInstance(Object* object, const ExactModelCoordinate& p, const std::string& id) {
		Location location(this);
		location.setExactLayerCoordinates(p);

		Instance* instance = new Instance(object, location, id);
		if(instance->isActive()) {
			setInstanceActivityStatus(instance, instance->isActive());
		}
		m_instances.push_back(instance);
		m_instanceTree->addInstance(instance);

		std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
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

	    Location& location = instance->getLocationRef();
		location.setLayer(this);
		location.setExactLayerCoordinates(p);

		m_instances.push_back(instance);
		m_instanceTree->addInstance(instance);
		if(instance->isActive()) {
			setInstanceActivityStatus(instance, instance->isActive());
		}

		std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			(*i)->onInstanceCreate(this, instance);
			++i;
		}
		m_changed = true;
		return true;
	}

	void Layer::removeInstance(Instance* instance) {
		// If the instance is changed and removed on the same pump,
		// it can happen that the instance can not cleanly be removed,
		// to avoid this we have to update the instance first and send
		// the result to the LayerChangeListeners.
		if (instance->isActive()) {
			if (instance->update() != ICHANGE_NO_CHANGES) {
				std::vector<Instance*> updateInstances;
				updateInstances.push_back(instance);
				std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
				while (i != m_changeListeners.end()) {
					(*i)->onLayerChanged(this, updateInstances);
					++i;
				}
			}
		}

		std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			(*i)->onInstanceDelete(this, instance);
			++i;
		}
		setInstanceActivityStatus(instance, false);
		std::vector<Instance*>::iterator it = m_instances.begin();
		for(; it != m_instances.end(); ++it) {
			if(*it == instance) {
				m_instanceTree->removeInstance(*it);
				m_instances.erase(it);
				break;
			}
		}
		m_changed = true;
	}

	void Layer::deleteInstance(Instance* instance) {
		// If the instance is changed and deleted on the same pump,
		// it can happen that the instance can not cleanly be removed,
		// to avoid this we have to update the instance first and send
		// the result to the LayerChangeListeners.
		if (instance->isActive()) {
			if (instance->update() != ICHANGE_NO_CHANGES) {
				std::vector<Instance*> updateInstances;
				updateInstances.push_back(instance);
				std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
				while (i != m_changeListeners.end()) {
					(*i)->onLayerChanged(this, updateInstances);
					++i;
				}
			}
		}

		std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
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

	const std::vector<Instance*>& Layer::getInstances() const {
		return m_instances;
	}

	void Layer::setInstanceActivityStatus(Instance* instance, bool active) {
		if(active) {
			m_activeInstances.insert(instance);
		} else {
			m_activeInstances.erase(instance);
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

	std::list<Instance*> Layer::getInstancesIn(Rect& rec) {
		std::list<Instance*> matching_instances;
		ModelCoordinate mc(rec.x, rec.y);
		m_instanceTree->findInstances(mc, rec.w, rec.h, matching_instances);

		return matching_instances;
	}

	std::vector<Instance*> Layer::getInstancesInLine(const ModelCoordinate& pt1, const ModelCoordinate& pt2) {
		std::vector<Instance*> instances;
		std::list<Instance*> matchingInstances;
		std::vector<ModelCoordinate> coords = m_grid->getCoordinatesInLine(pt1, pt2);
		for (std::vector<ModelCoordinate>::iterator it = coords.begin(); it != coords.end(); ++it) {
			m_instanceTree->findInstances(*it, 0, 0, matchingInstances);
			if (!matchingInstances.empty()) {
				instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
			}
		}
		return instances;
	}

	std::vector<Instance*> Layer::getInstancesInCircle(const ModelCoordinate& center, uint16_t radius) {
		std::vector<Instance*> instances;
		std::list<Instance*> matchingInstances;
		//radius power 2
		uint16_t radiusp2 = (radius+1) * radius;

		ModelCoordinate current(center.x-radius, center.y-radius);
		ModelCoordinate target(center.x+radius, center.y+radius);
		for (; current.y < center.y; current.y++) {
			current.x = center.x-radius;
			for (; current.x < center.x; current.x++) {
				uint16_t dx = center.x - current.x;
				uint16_t dy = center.y - current.y;
				uint16_t distance = dx*dx + dy*dy;
				if (distance <= radiusp2) {
					m_instanceTree->findInstances(current, 0, 0, matchingInstances);
					if (!matchingInstances.empty()) {
						instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
					}

					current.x = center.x + dx;
					m_instanceTree->findInstances(current, 0, 0, matchingInstances);
					if (!matchingInstances.empty()) {
						instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
					}

					current.y = center.y + dy;
					m_instanceTree->findInstances(current, 0, 0, matchingInstances);
					if (!matchingInstances.empty()) {
						instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
					}

					current.x = center.x-dx;
					m_instanceTree->findInstances(current, 0, 0, matchingInstances);
					if (!matchingInstances.empty()) {
						instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
					}

					current.y = center.y-dy;
				}
			}
		}
		current.x = center.x;
		current.y = center.y-radius;
		for (; current.y <= target.y; current.y++) {
			m_instanceTree->findInstances(current, 0, 0, matchingInstances);
			if (!matchingInstances.empty()) {
				instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
			}
		}

		current.y = center.y;
		current.x = center.x-radius;
		for (; current.x <= target.x; current.x++) {
			m_instanceTree->findInstances(current, 0, 0, matchingInstances);
			if (!matchingInstances.empty()) {
				instances.insert(instances.end(), matchingInstances.begin(), matchingInstances.end());
			}
		}
		return instances;
	}

	std::vector<Instance*> Layer::getInstancesInCircleSegment(const ModelCoordinate& center, uint16_t radius, int32_t sangle, int32_t eangle) {
		std::vector<Instance*> instances;
		ExactModelCoordinate exactCenter(center.x, center.y);
		std::vector<Instance*> tmpInstances = getInstancesInCircle(center, radius);
		int32_t s = (sangle + 360) % 360;
		int32_t e = (eangle + 360) % 360;
		bool greater = (s > e) ? true : false;
		for (std::vector<Instance*>::iterator it = tmpInstances.begin(); it != tmpInstances.end(); ++it) {
			int32_t angle = getAngleBetween(exactCenter, intPt2doublePt((*it)->getLocationRef().getLayerCoordinates()));
			if (greater) {
				if (angle >= s || angle <= e) {
					instances.push_back(*it);
				}
			} else {
				if (angle >= s && angle <= e) {
					instances.push_back(*it);
				}
			}
		}
		return instances;
	}

	void Layer::getMinMaxCoordinates(ModelCoordinate& min, ModelCoordinate& max, const Layer* layer) const {
		if (!layer) {
			layer = this;
		}

		if (m_instances.empty()) {
			min = ModelCoordinate();
			max = min;
		} else {
			min = m_instances.front()->getLocationRef().getLayerCoordinates(layer);
			max = min;

			for (std::vector<Instance*>::const_iterator i = m_instances.begin(); i != m_instances.end(); ++i) {
				ModelCoordinate coord = (*i)->getLocationRef().getLayerCoordinates(layer);
				min.x = std::min(min.x, coord.x);
				max.x = std::max(max.x, coord.x);
				min.y = std::min(min.y, coord.y);
				max.y = std::max(max.y, coord.y);
			}
		}

	}

	float Layer::getZOffset() const {
		static const float globalmax = 100.0;
		static const float globalrange = 200.0;
		int32_t numlayers = m_map->getLayerCount();
		int32_t thislayer = 1; // we don't need 0 indexed

		const std::list<Layer*>& layers = m_map->getLayers();
		std::list<Layer*>::const_iterator iter = layers.begin();
		for (; iter != layers.end(); ++iter, ++thislayer) {
			if (*iter == this) {
				break;
			}
		}

		float offset = globalmax - (numlayers - (thislayer - 1)) * (globalrange/numlayers);
		return offset;
	}

	uint32_t Layer::getLayerCount() const {
		return m_map->getLayerCount();
	}

	void Layer::setInstancesVisible(bool vis) {
		if (m_instancesVisibility != vis) {
			m_instancesVisibility = vis;
			std::vector<Instance*>::iterator it = m_instances.begin();
			for (; it != m_instances.end(); ++it) {
				(*it)->callOnVisibleChange();
			}
		}
	}

	void Layer::setLayerTransparency(uint8_t transparency) {
		if (m_transparency != transparency) {
			m_transparency = transparency;
			std::vector<Instance*>::iterator it = m_instances.begin();
			for (; it != m_instances.end(); ++it) {
				(*it)->callOnTransparencyChange();
			}
		}
	}

	uint8_t Layer::getLayerTransparency() {
		return m_transparency;
	}

	void Layer::toggleInstancesVisible() {
		setInstancesVisible(!m_instancesVisibility);
	}

	bool Layer::areInstancesVisible() const {
		return m_instancesVisibility;
	}

	bool Layer::cellContainsBlockingInstance(const ModelCoordinate& cellCoordinate) {
		bool blockingInstance = false;
		if (m_cellCache) {
			Cell* cell = m_cellCache->getCell(cellCoordinate);
			if (cell) {
				return cell->getCellType() != CTYPE_NO_BLOCKER;
			}
		} else {
			std::list<Instance*> adjacentInstances;
			m_instanceTree->findInstances(cellCoordinate, 0, 0, adjacentInstances);
			for(std::list<Instance*>::const_iterator j = adjacentInstances.begin(); j != adjacentInstances.end(); ++j) {
				if((*j)->isBlocking() && (*j)->getLocationRef().getLayerCoordinates() == cellCoordinate) {
					blockingInstance = true;
					break;
				}
			}
		}
		return blockingInstance;
	}

	std::vector<Instance*> Layer::getBlockingInstances(const ModelCoordinate& cellCoordinate) {
		std::vector<Instance*> blockingInstances;
		if (m_cellCache) {
			Cell* cell = m_cellCache->getCell(cellCoordinate);
			if (cell) {
				const std::set<Instance*>& blocker = cell->getInstances();
				for (std::set<Instance*>::const_iterator it = blocker.begin(); it != blocker.end(); ++it) {
					if ((*it)->isBlocking()) {
						blockingInstances.push_back(*it);
					}
				}
			}
		} else {
			std::list<Instance*> adjacentInstances;
			m_instanceTree->findInstances(cellCoordinate, 0, 0, adjacentInstances);
			for(std::list<Instance*>::const_iterator j = adjacentInstances.begin(); j != adjacentInstances.end(); ++j) {
				if((*j)->isBlocking() && (*j)->getLocationRef().getLayerCoordinates() == cellCoordinate) {
					blockingInstances.push_back(*j);
				}
			}
		}
		return blockingInstances;
	}

	void Layer::setPathingStrategy(PathingStrategy strategy) {
		m_pathingStrategy = strategy;
		m_grid->setAllowDiagonals(m_pathingStrategy != CELL_EDGES_ONLY);
	}

	PathingStrategy Layer::getPathingStrategy() const {
		return m_pathingStrategy;
	}

	void Layer::setSortingStrategy(SortingStrategy strategy) {
		m_sortingStrategy = strategy;
	}

	SortingStrategy Layer::getSortingStrategy() const {
		return m_sortingStrategy;
	}

	void Layer::setWalkable(bool walkable) {
		m_walkable = walkable;
	}

	bool Layer::isWalkable() {
		return m_walkable;
	}

	void Layer::setInteract(bool interact, const std::string& id) {
		m_interact = interact;
		m_walkableId = id;
	}

	bool Layer::isInteract() {
		return m_interact;
	}

	const std::string& Layer::getWalkableId() {
		return m_walkableId;
	}

	void Layer::addInteractLayer(Layer* layer) {
		if (m_walkable) {
			m_interacts.push_back(layer);
		}
	}

	const std::vector<Layer*>& Layer::getInteractLayers() {
		return m_interacts;
	}

	void Layer::removeInteractLayer(Layer* layer) {
		if (m_walkable) {
			std::vector<Layer*>::iterator it = m_interacts.begin();
			for (; it != m_interacts.end(); ++it) {
				if (*it == layer) {
					(*it)->removeChangeListener(m_cellCache->getCellCacheChangeListener());
					m_interacts.erase(it);
					break;
				}
			}
		}
	}

	void Layer::createCellCache() {
		if (!m_cellCache && m_walkable) {
			m_cellCache = new CellCache(this);
		}
	}

	CellCache* Layer::getCellCache() {
		return m_cellCache;
	}

	void Layer::destroyCellCache() {
		if (m_walkable) {
			removeChangeListener(m_cellCache->getCellCacheChangeListener());
			if (!m_interacts.empty()) {
				std::vector<Layer*>::iterator it = m_interacts.begin();
				for (; it != m_interacts.end(); ++it) {
					(*it)->removeChangeListener(m_cellCache->getCellCacheChangeListener());
					(*it)->setInteract(false, "");
				}
				m_interacts.clear();
			}
			delete m_cellCache;
			m_cellCache = NULL;
			m_walkable = false;
		}
	}

	bool Layer::update() {
		m_changedInstances.clear();
		std::vector<Instance*> inactiveInstances;
		std::set<Instance*>::iterator it = m_activeInstances.begin();
		for(; it != m_activeInstances.end(); ++it) {
			if ((*it)->update() != ICHANGE_NO_CHANGES) {
				m_changedInstances.push_back(*it);
				m_changed = true;
			} else if (!(*it)->isActive()) {
				inactiveInstances.push_back(*it);
			}
		}
		if (!m_changedInstances.empty()) {
			std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
			while (i != m_changeListeners.end()) {
				(*i)->onLayerChanged(this, m_changedInstances);
				++i;
			}
			//std::cout << "Layer named " << Id() << " changed = 1\n";
		}
		// remove inactive instances from m_activeInstances
		if (!inactiveInstances.empty()) {
			std::vector<Instance*>::iterator i = inactiveInstances.begin();
			while (i != inactiveInstances.end()) {
				m_activeInstances.erase(*i);
				++i;
			}
		}
		//std::cout << "Layer named " << Id() << " changed = 0\n";
		bool retval = m_changed;
		m_changed = false;
		return retval;
	}

	void Layer::addChangeListener(LayerChangeListener* listener) {
		m_changeListeners.push_back(listener);
	}

	void Layer::removeChangeListener(LayerChangeListener* listener) {
		std::vector<LayerChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			if ((*i) == listener) {
				m_changeListeners.erase(i);
				return;
			}
			++i;
		}
	}

	bool Layer::isChanged() {
		return m_changed;
	}

	std::vector<Instance*>& Layer::getChangedInstances() {
		return m_changedInstances;
	}

	void Layer::setStatic(bool stati) {
		m_static = stati;
	}

	bool Layer::isStatic() {
		return m_static;
	}

} // FIFE
