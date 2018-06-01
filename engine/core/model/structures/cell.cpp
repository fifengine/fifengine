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
#include "view/visual.h"

#include "cell.h"
#include "cellcache.h"
#include "instance.h"
#include "layer.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	Cell::Cell(int32_t coordint, ModelCoordinate coordinate, Layer* layer):
		m_coordId(coordint),
		m_coordinate(coordinate),
		m_layer(layer),
		m_zone(NULL),
		m_transition(NULL),
		m_inserted(false),
		m_protect(false) {
	}

	Cell::~Cell() {
		// calls CellDeleteListener, e.g. for transition
		if (!m_deleteListeners.empty()) {
			std::vector<CellDeleteListener*>::iterator it = m_deleteListeners.begin();
			for (; it != m_deleteListeners.end(); ++it) {
				if (*it) {
					(*it)->onCellDeleted(this);
				}
			}
		}
		// remove cell from zone
		if (m_zone) {
			m_zone->removeCell(this);
		}
		// delete m_transition;
		if (m_transition) {
			deleteTransition();
		}
		// remove cell from cache (costs, narrow, area)
		m_layer->getCellCache()->removeCell(this);
	}

	void Cell::addInstances(const std::list<Instance*>& instances) {
		CellCache* cache = m_layer->getCellCache();
		for (std::list<Instance*>::const_iterator it = instances.begin(); it != instances.end(); ++it) {
			std::pair<std::set<Instance*>::iterator, bool> ret = m_instances.insert(*it);
			if (ret.second) {
				if ((*it)->isSpecialCost()) {
					cache->registerCost((*it)->getCostId(), (*it)->getCost());
					cache->addCellToCost((*it)->getCostId(), this);
				}
				if ((*it)->isSpecialSpeed()) {
					cache->setSpeedMultiplier(this, (*it)->getSpeed());
				}
				if ((*it)->getObject()->getArea() != "") {
					cache->addCellToArea((*it)->getObject()->getArea(), this);
				}
				callOnInstanceEntered(*it);
			}
		}
		updateCellBlockingInfo();
	}

	void Cell::addInstance(Instance* instance) {
		std::pair<std::set<Instance*>::iterator, bool> ret = m_instances.insert(instance);
		if (ret.second) {
			CellCache* cache = m_layer->getCellCache();
			if (instance->isSpecialCost()) {
				cache->registerCost(instance->getCostId(), instance->getCost());
				cache->addCellToCost(instance->getCostId(), this);
			}
			if (instance->isSpecialSpeed()) {
				cache->setSpeedMultiplier(this, instance->getSpeed());
			}
			if (instance->getObject()->getArea() != "") {
				cache->addCellToArea(instance->getObject()->getArea(), this);
			}
			callOnInstanceEntered(instance);
			updateCellBlockingInfo();
		}
	}

	void Cell::changeInstance(Instance* instance) {
		updateCellBlockingInfo();
	}

	void Cell::removeInstance(Instance* instance) {
		if (m_instances.erase(instance) == 0) {
			FL_ERR(_log, "Tried to remove an instance from cell, but given instance could not be found.");
			return;
		}
		CellCache* cache = m_layer->getCellCache();
		if (instance->isSpecialCost()) {
			cache->removeCellFromCost(instance->getCostId(), this);
		}
		if (instance->isSpecialSpeed()) {
			cache->resetSpeedMultiplier(this);
			// try to find other speed value
			if (!m_instances.empty()) {
				std::set<Instance*>::iterator it = m_instances.begin();
				for (; it != m_instances.end(); ++it) {
					if ((*it)->isSpecialSpeed()) {
						cache->setSpeedMultiplier(this, (*it)->getSpeed());
						break;
					}
				}
			}
		}
		if (instance->getObject()->getArea() != "") {
			cache->removeCellFromArea(instance->getObject()->getArea(), this);
		}
		callOnInstanceExited(instance);
		updateCellBlockingInfo();
	}

	bool Cell::isNeighbor(Cell* cell) {
		std::vector<Cell*>::iterator it = m_neighbors.begin();
		for (; it != m_neighbors.end(); ++it) {
			if (*it == cell) {
				return true;
			}
		}
		return false;
	}

	void Cell::updateCellBlockingInfo() {
		CellTypeInfo old_type = m_type;
		m_coordinate.z = MIN_CELL_Z;
		if (!m_instances.empty()) {
			int32_t pos = -1;
			bool cellblock = (m_type == CTYPE_CELL_NO_BLOCKER || m_type == CTYPE_CELL_BLOCKER);
			for (std::set<Instance*>::iterator it = m_instances.begin(); it != m_instances.end(); ++it) {
				if (cellblock) {
					continue;
				}
				uint8_t stackpos = (*it)->getCellStackPosition();
				if (stackpos < pos) {
					continue;
				}
				// update cell z
				if (m_coordinate.z < (*it)->getLocationRef().getLayerCoordinates().z && (*it)->getObject()->isStatic()) {
					m_coordinate.z = (*it)->getLocationRef().getLayerCoordinates().z;
				}
				if ((*it)->getCellStackPosition() > pos) {
					pos = (*it)->getCellStackPosition();
					if ((*it)->isBlocking()) {
						if (!(*it)->getObject()->isStatic()) {
							m_type = CTYPE_DYNAMIC_BLOCKER;
						} else {
							m_type = CTYPE_STATIC_BLOCKER;
						}
					} else {
						m_type = CTYPE_NO_BLOCKER;
					}
				} else {
					// if positions are equal then static_blockers win
					if ((*it)->isBlocking() && m_type != CTYPE_STATIC_BLOCKER) {
						if (!(*it)->getObject()->isStatic()) {
							m_type = CTYPE_DYNAMIC_BLOCKER;
						} else {
							m_type = CTYPE_STATIC_BLOCKER;
						}
					}
				}
			}
		} else {
			if (m_type == CTYPE_STATIC_BLOCKER || m_type == CTYPE_DYNAMIC_BLOCKER) {
				m_type = CTYPE_NO_BLOCKER;
			}
		}
		if (Mathd::Equal(m_coordinate.z, MIN_CELL_Z)) {
			m_coordinate.z = 0;
		}

		if (old_type != m_type) {
			bool block = (m_type == CTYPE_STATIC_BLOCKER ||
				m_type == CTYPE_DYNAMIC_BLOCKER || m_type == CTYPE_CELL_BLOCKER);
			m_layer->getCellCache()->setBlockingUpdate(true);
			callOnBlockingChanged(block);
		}
	}

	void Cell::updateCellInfo() {
		updateCellBlockingInfo();

		if (!m_deleteListeners.empty()) {
			m_deleteListeners.erase(
				std::remove(m_deleteListeners.begin(), m_deleteListeners.end(),
				(CellDeleteListener*)NULL),	m_deleteListeners.end());
		}
		if (!m_changeListeners.empty()) {
			m_changeListeners.erase(
				std::remove(m_changeListeners.begin(), m_changeListeners.end(),
				(CellChangeListener*)NULL),	m_changeListeners.end());
		}
	}

	bool Cell::defaultCost() {
		return m_layer->getCellCache()->isDefaultCost(this);
	}

	void Cell::setCostMultiplier(double multi) {
		m_layer->getCellCache()->setCostMultiplier(this, multi);
	}

	double Cell::getCostMultiplier() {
		return m_layer->getCellCache()->getCostMultiplier(this);
	}

	void Cell::resetCostMultiplier() {
		m_layer->getCellCache()->resetCostMultiplier(this);
	}

	bool Cell::defaultSpeed() {
		return m_layer->getCellCache()->isDefaultSpeed(this);
	}

	void Cell::setSpeedMultiplier(double multi) {
		m_layer->getCellCache()->setSpeedMultiplier(this, multi);
	}

	double Cell::getSpeedMultiplier() {
		return m_layer->getCellCache()->getSpeedMultiplier(this);
	}

	void Cell::resetSpeedMultiplier() {
		m_layer->getCellCache()->resetSpeedMultiplier(this);
	}

	Zone* Cell::getZone() {
		return m_zone;
	}

	void Cell::setZone(Zone* zone) {
		m_zone = zone;
	}

	void Cell::resetZone() {
		m_inserted = false;
		m_zone = NULL;
	}

	bool Cell::isInserted() {
		return m_inserted;
	}

	void Cell::setInserted(bool inserted) {
		m_inserted = inserted;
	}

	bool Cell::isZoneProtected() {
		return m_protect;
	}

	void Cell::setZoneProtected(bool protect) {
		m_protect = protect;
	}

	CellTypeInfo Cell::getCellType() {
		return m_type;
	}

	void Cell::setCellType(CellTypeInfo type) {
		m_type = type;
	}

	const std::set<Instance*>& Cell::getInstances() {
		return m_instances;
	}

	void Cell::setCellId(int32_t id) {
		m_coordId = id;
	}

	int32_t Cell::getCellId() {
		return m_coordId;
	}

	const ModelCoordinate Cell::getLayerCoordinates() const {
		return m_coordinate;
	}

	void Cell::addNeighbor(Cell* cell) {
		m_neighbors.push_back(cell);
	}

	const std::vector<Cell*>& Cell::getNeighbors() {
		return m_neighbors;
	}

	void Cell::resetNeighbors() {
		m_neighbors.clear();
		if (m_transition) {
			CellCache* cache = m_transition->m_layer->getCellCache();
			if (cache) {
				Cell* cell = cache->getCell(m_transition->m_mc);
				if (cell) {
					m_neighbors.push_back(cell);
				}
			}
		}
	}

	Layer* Cell::getLayer() {
		return m_layer;
	}

	void Cell::createTransition(Layer* layer, const ModelCoordinate& mc, bool immediate) {
		TransitionInfo* trans = new TransitionInfo(layer);
		// if layers are the same then it's a portal
		if (layer != m_layer) {
			trans->m_difflayer = true;
		}
		trans->m_immediate = immediate;
		trans->m_mc = mc;

		deleteTransition();

		m_transition = trans;

		Cell* c = layer->getCellCache()->getCell(mc);
		if (c) {
			m_neighbors.push_back(c);
			c->addDeleteListener(this);
			m_layer->getCellCache()->addTransition(this);
		} else {
			delete m_transition;
			m_transition = NULL;
		}
	}

	void Cell::deleteTransition() {
		if (m_transition) {
			Cell* oldc = m_transition->m_layer->getCellCache()->getCell(m_transition->m_mc);
			std::vector<Cell*>::iterator it = m_neighbors.begin();
			for (; it != m_neighbors.end(); ++it) {
				if (*it == oldc) {
					m_neighbors.erase(it);
					break;
				}
			}
			oldc->removeDeleteListener(this);
			m_layer->getCellCache()->removeTransition(this);
			delete m_transition;
			m_transition = NULL;
		}
	}

	TransitionInfo* Cell::getTransition() {
		return m_transition;
	}

	void Cell::addDeleteListener(CellDeleteListener* listener) {
		m_deleteListeners.push_back(listener);
	}

	void Cell::removeDeleteListener(CellDeleteListener* listener) {
		std::vector<CellDeleteListener*>::iterator it = m_deleteListeners.begin();
		for (; it != m_deleteListeners.end(); ++it) {
			if (*it == listener) {
				*it = NULL;
				break;
			}
		}
	}

	void Cell::onCellDeleted(Cell* cell) {
		std::vector<Cell*>::iterator it = m_neighbors.begin();
		for (; it != m_neighbors.end(); ++it) {
			if (*it == cell) {
				deleteTransition();
				break;
			}
		}
	}

	void Cell::addChangeListener(CellChangeListener* listener) {
		m_changeListeners.push_back(listener);
	}

	void Cell::removeChangeListener(CellChangeListener* listener) {
		std::vector<CellChangeListener*>::iterator it = m_changeListeners.begin();
		for (; it != m_changeListeners.end(); ++it) {
			if (*it == listener) {
				*it = NULL;
				break;
			}
		}
	}

	void Cell::callOnInstanceEntered(Instance* instance) {
		if (m_changeListeners.empty()) {
			return;
		}

		std::vector<CellChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			if (*i) {
				(*i)->onInstanceEnteredCell(this, instance);
			}
			++i;
		}
	}

	void Cell::callOnInstanceExited(Instance* instance) {
		if (m_changeListeners.empty()) {
			return;
		}

		std::vector<CellChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			if (*i) {
				(*i)->onInstanceExitedCell(this, instance);
			}
			++i;
		}
	}

	void Cell::callOnBlockingChanged(bool blocks) {
		if (m_changeListeners.empty()) {
			return;
		}

		std::vector<CellChangeListener*>::iterator i = m_changeListeners.begin();
		while (i != m_changeListeners.end()) {
			if (*i) {
				(*i)->onBlockingChangedCell(this, m_type, blocks);
			}
			++i;
		}
	}
} // FIFE
