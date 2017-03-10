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
#include "model/metamodel/modelcoords.h"
#include "util/log/logger.h"

#include "cell.h"
#include "cellcache.h"
#include "map.h"
#include "trigger.h"
#include "triggercontroller.h"

namespace FIFE {

	static Logger _log(LM_STRUCTURES);

	TriggerController::TriggerController(Map* map) :
		m_map(map) {
	}

	TriggerController::~TriggerController() {
		TriggerNameMapIterator it = m_triggerNameMap.begin();
		for (; it != m_triggerNameMap.end(); ++it) {
			delete it->second;
		}
	}

	Trigger* TriggerController::createTrigger(const std::string& triggerName) {
		//assert(!exists(triggerName));

		Trigger* trigger = new Trigger(triggerName);

		std::pair<TriggerNameMapIterator, bool> returnValue;
		returnValue = m_triggerNameMap.insert ( TriggerNameMapPair(triggerName, trigger));

		if (!returnValue.second) {
			delete trigger;
			FL_WARN(_log, LMsg("TriggerController::createTrigger() - ") << "Trigger " << triggerName << " already exists.... ignoring.");
		}

		return returnValue.first->second;
	}

	Trigger* TriggerController::createTriggerOnCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt) {
		assert(layer);

		Trigger* trigger = createTrigger(triggerName);
		trigger->assign(layer, pt);
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords) {
		assert(layer);

		Trigger* trigger = createTrigger(triggerName);
		for (std::vector<ModelCoordinate>::const_iterator it = coords.begin(); it != coords.end(); ++it) {
			trigger->assign(layer, *it);
		}
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnRect(const std::string& triggerName, Layer* layer, const Rect& rec) {
		assert(layer);
		assert(layer->getCellCache());

		Trigger* trigger = createTrigger(triggerName);
		std::vector<Cell*> cells = layer->getCellCache()->getCellsInRect(rec);
		for (std::vector<Cell*>::iterator it = cells.begin(); it != cells.end(); ++it) {
			trigger->assign(*it);
		}
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnLocation(const std::string& triggerName, const Location& loc) {
		assert(loc.getLayer());
		assert(loc.getLayer()->getCellCache());

		Trigger* trigger = createTrigger(triggerName);
		trigger->assign(loc.getLayer(), loc.getLayerCoordinates());
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnLocations(const std::string& triggerName, const std::vector<Location>& locs) {
		Trigger* trigger = createTrigger(triggerName);
		for (std::vector<Location>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
			trigger->assign((*it).getLayer(), (*it).getLayerCoordinates());
		}
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnCell(const std::string& triggerName, Cell* cell) {
		assert(cell);

		Trigger* trigger = createTrigger(triggerName);
		trigger->assign(cell);
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnCells(const std::string& triggerName, const std::vector<Cell*>& cells) {
		Trigger* trigger = createTrigger(triggerName);
		for (std::vector<Cell*>::const_iterator it = cells.begin(); it != cells.end(); ++it) {
			trigger->assign(*it);
		}
		return trigger;
	}

	Trigger* TriggerController::createTriggerOnInstance(const std::string& triggerName, Instance* instance) {
		assert(instance);

		Trigger* trigger = createTrigger(triggerName);
		trigger->attach(instance);
		return trigger;
	}

	Trigger* TriggerController::getTrigger(const std::string& triggerName) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			return it->second;
		}
		return NULL;
	}

	void TriggerController::deleteTrigger(const std::string& triggerName) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			delete it->second;
			m_triggerNameMap.erase(it);
		}
	}

	void TriggerController::removeTriggerFromCoordinate(const std::string& triggerName, Layer* layer, const ModelCoordinate& pt) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			it->second->remove(layer, pt);
		}
	}

	void TriggerController::removeTriggerFromCoordinates(const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			for (std::vector<ModelCoordinate>::const_iterator cit = coords.begin(); cit != coords.end(); ++cit) {
				it->second->remove(layer, *cit);
			}
		}
	}

	void TriggerController::removeTriggerFromRect(const std::string& triggerName, Layer* layer, const Rect& rec) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			std::vector<Cell*> cells = layer->getCellCache()->getCellsInRect(rec);
			for (std::vector<Cell*>::iterator cit = cells.begin(); cit != cells.end(); ++cit) {
				it->second->remove(*cit);
			}
		}
	}

	void TriggerController::removeTriggerFromLocation(const std::string& triggerName, const Location& loc) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			it->second->remove(loc.getLayer(), loc.getLayerCoordinates());
		}
	}

	void TriggerController::removeTriggerFromLocations(const std::string& triggerName, const std::vector<Location>& locs) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			for (std::vector<Location>::const_iterator cit = locs.begin(); cit != locs.end(); ++cit) {
				it->second->remove((*cit).getLayer(), (*cit).getLayerCoordinates());
			}
		}
	}

	void TriggerController::removeTriggerFromCell(const std::string& triggerName, Cell* cell) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			it->second->remove(cell);
		}
	}

	void TriggerController::removeTriggerFromCells(const std::string& triggerName, const std::vector<Cell*>& cells) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			for (std::vector<Cell*>::const_iterator cit = cells.begin(); cit != cells.end(); ++cit) {
				it->second->remove(*cit);
			}
		}
	}

	void TriggerController::removeTriggerFromInstance(const std::string& triggerName, Instance* instance) {
		TriggerNameMapIterator it = m_triggerNameMap.find(triggerName);
		if (it != m_triggerNameMap.end()) {
			if (it->second->getAttached() == instance) {
				it->second->detach();
			}
		}
	}

	std::vector<Trigger*> TriggerController::getAllTriggers() {
		std::vector<Trigger*> triggers;
		TriggerNameMapIterator it = m_triggerNameMap.begin();
		for (; it != m_triggerNameMap.end(); ++it) {
			triggers.push_back(it->second);
		}
		return triggers;
	}

	std::vector<std::string> TriggerController::getAllTriggerNames() {
		std::vector<std::string> names;
		TriggerNameMapIterator it = m_triggerNameMap.begin();
		for (; it != m_triggerNameMap.end(); ++it) {
			names.push_back(it->first);
		}
		return names;
	}

	bool TriggerController::exists(const std::string& name) {
		TriggerNameMapIterator it = m_triggerNameMap.find(name);
		if (it != m_triggerNameMap.end()) {
			return true;
		}

		return false;
	}

} //FIFE
