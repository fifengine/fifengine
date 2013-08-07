/***************************************************************************
 *   Copyright (C) 2006-2012 by the FIFE team                              *
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
#include "util/base/exception.h"

#include "object.h"
#include "action.h"
#include "ipather.h"

namespace FIFE {

	Object::BasicObjectProperty::BasicObjectProperty():
		m_area(""),
		m_actions(NULL),
		m_defaultAction(NULL),
		m_blocking(false),
		m_static(false),
		m_cellStack(0) {
	}
	Object::BasicObjectProperty::~BasicObjectProperty() {
		if (m_actions) {
			std::map<std::string, Action*>::const_iterator i(m_actions->begin());
			while (i != m_actions->end()) {
				delete i->second;
				++i;
			}
			delete m_actions;
		}
	}

	Object::MovableObjectProperty::MovableObjectProperty():
		m_pather(NULL),
		m_costId(""),
		m_cost(1.0),
		m_speed(1.0),
		m_zRange(0) {
	}
	Object::MovableObjectProperty::~MovableObjectProperty() {
	}

	Object::MultiObjectProperty::MultiObjectProperty():
		m_multiPart(false),
		m_restrictedRotation(false) {
	}
	Object::MultiObjectProperty::~MultiObjectProperty() {
	}

	Object::Object(const std::string& identifier, const std::string& name_space, Object* inherited):
		m_id(identifier),
		m_namespace(name_space),
		m_filename(""),
		m_inherited(inherited),
		m_visual(NULL),
		m_basicProperty(NULL),
		m_moveProperty(NULL),
		m_multiProperty(NULL) {
	}

	Object::~Object() {
		delete m_visual;
		delete m_basicProperty;
		delete m_moveProperty;
		delete m_multiProperty;
	}

	Action* Object::createAction(const std::string& identifier, bool is_default) {
		std::map<std::string, Action*>* actions;
		if (!m_basicProperty) {
			m_basicProperty = new BasicObjectProperty();
		}
		
		if (!m_basicProperty->m_actions) {
			m_basicProperty->m_actions = new std::map<std::string, Action*>;
		}
		actions = m_basicProperty->m_actions;

		std::map<std::string, Action*>::const_iterator it = actions->begin();
		for(; it != actions->end(); ++it) {
			if(identifier == it->second->getId()) {
				throw NameClash(identifier);
			}
		}

		Action* a = getAction(identifier, false);
		if (!a) {
			a = new Action(identifier);
			(*actions)[identifier] = a;
			if (is_default || (!m_basicProperty->m_defaultAction)) {
				m_basicProperty->m_defaultAction = a;
			}
		}
		return a;
	}

	Action* Object::getAction(const std::string& identifier, bool deepsearch) const {
		std::map<std::string, Action*>* actions = NULL;
		if (m_basicProperty) {
			actions = m_basicProperty->m_actions;
		}

		std::map<std::string, Action*>::const_iterator i;
		if (actions) {
			i = actions->find(identifier);
		}
		if ((!actions) || (i == actions->end())) {
			if (m_inherited && deepsearch) {
				return m_inherited->getAction(identifier);
			}
			return NULL;
		}
		return i->second;
	}

	std::list<std::string> Object::getActionIds() const {
		std::map<std::string, Action*>* actions = NULL;
		if (m_basicProperty) {
			actions = m_basicProperty->m_actions;
		}
		std::list<std::string> action_ids;
		if (actions) {
			std::map<std::string, Action*>::const_iterator actions_it = actions->begin();
			for(; actions_it != actions->end(); ++actions_it) {
				action_ids.push_back(actions_it->first);
			}
		}
		return action_ids;
	}

	void Object::setDefaultAction(const std::string& identifier) {
		std::map<std::string, Action*>::const_iterator i;
		Action* action = NULL;
		std::map<std::string, Action*>* actions = NULL;
		if (m_basicProperty) {
			actions = m_basicProperty->m_actions;
		}
		if (actions) {
			i = actions->find(identifier);
		}
		if ((!actions) || (i == actions->end())) {
			if (m_inherited) {
				action = m_inherited->getAction(identifier);
			}
		} else {
			action = i->second;
		}

		if (action && m_basicProperty) {
			m_basicProperty->m_defaultAction = action;
		}
	}

	Action* Object::getDefaultAction() const {
		if (m_basicProperty) {
			return m_basicProperty->m_defaultAction;
		}
		if (m_inherited) {
			return m_inherited->getDefaultAction();
		}
		return NULL;
	}

	void Object::setPather(IPather* pather) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_pather = pather;
	}

	IPather* Object::getPather() const {
		if (m_moveProperty) {
			return m_moveProperty->m_pather;
		}
		if (m_inherited) {
			return m_inherited->getPather();
		}
		return NULL;
	}

	Object* Object::getInherited() const {
		return m_inherited;
	}

	void Object::adoptVisual(IVisual* visual) {
		if (m_visual && m_visual != visual) {
			delete m_visual;
		}
		m_visual = visual;
	}

	void Object::setBlocking(bool blocking) {
		if (!m_basicProperty) {
			m_basicProperty = new BasicObjectProperty();
		}
		m_basicProperty->m_blocking = blocking;
	}

	bool Object::isBlocking() const {
		if (m_basicProperty) {
			return m_basicProperty->m_blocking;
		}
		if (m_inherited) {
			return m_inherited->isBlocking();
		}
		return false;
	}

	void Object::setStatic(bool stat) {
		if (!m_basicProperty) {
			m_basicProperty = new BasicObjectProperty();
		}
		m_basicProperty->m_static = stat;
	}

	bool Object::isStatic() const {
		if (!m_basicProperty) {
			if (m_inherited) {
				return m_inherited->isStatic();
			}
			return false;
		}
		return m_basicProperty->m_static;
	}

	void Object::setFilename(const std::string& file) {
		m_filename = file;
	}

	const std::string& Object::getFilename() const {
		return m_filename;
	}

	void Object::setCellStackPosition(uint8_t position) {
		if (!m_basicProperty) {
			m_basicProperty = new BasicObjectProperty();
		}
		m_basicProperty->m_cellStack = position;
	}

	uint8_t Object::getCellStackPosition() const {
		if (m_basicProperty) {
			return m_basicProperty->m_cellStack;
		}
		if (m_inherited) {
			return m_inherited->getCellStackPosition();
		}
		return 0;
	}

	bool Object::isSpecialCost() const {
		if (m_moveProperty) {
			return m_moveProperty->m_costId != "";
		}
		if (m_inherited) {
			return m_inherited->isSpecialCost();
		}
		return false;
	}

	void Object::setCostId(const std::string& cost) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_costId = cost;
	}

	std::string Object::getCostId() const {
		if (m_moveProperty) {
			return m_moveProperty->m_costId;
		}
		if (m_inherited) {
			return m_inherited->getCostId();
		}
		return "";
	}

	void Object::setCost(double cost) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_cost = cost;
	}
	
	double Object::getCost() const {
		if (m_moveProperty) {
			return m_moveProperty->m_cost;
		}
		if (m_inherited) {
			return m_inherited->getCost();
		}
		return 1.0;
	}

	bool Object::isSpecialSpeed() const {
		if (m_moveProperty) {
			return !Mathd::Equal(m_moveProperty->m_speed, 1.0);
		}
		if (m_inherited) {
			return m_inherited->isSpecialSpeed();
		}
		return false;
	}

	void Object::setSpeed(double speed) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_speed = speed;
	}

	double Object::getSpeed() const {
		if (m_moveProperty) {
			return m_moveProperty->m_speed;
		}
		if (m_inherited) {
			return m_inherited->getSpeed();
		}
		return 1.0;
	}

	bool Object::isMultiObject() const {
		if (m_multiProperty) {
			return !m_multiProperty->m_multiPartIds.empty();
		}
		if (m_inherited) {
			return m_inherited->isMultiObject();
		}
		return false;
	}

	void Object::addMultiPartId(const std::string& partId) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_multiPartIds.push_back(partId);
	}

	std::list<std::string> Object::getMultiPartIds() const {
		if (m_multiProperty) {
			return m_multiProperty->m_multiPartIds;
		}
		if (m_inherited) {
			return m_inherited->getMultiPartIds();
		}
		return std::list<std::string>();
	}

	void Object::removeMultiPartId(const std::string& partId) {
		if (!m_multiProperty) {
			return;
		}
		std::list<std::string>::iterator it = m_multiProperty->m_multiPartIds.begin();
		for (; it != m_multiProperty->m_multiPartIds.end(); ++it) {
			if (*it == partId) {
				m_multiProperty->m_multiPartIds.erase(it);
				break;
			}
		}
	}
	
	void Object::removeAllMultiPartIds() {
		if (!m_multiProperty) {
			return;
		}
		m_multiProperty->m_multiPartIds.clear();
	}

	bool Object::isMultiPart() const {
		if (m_multiProperty) {
			return m_multiProperty->m_multiPart;
		}
		if (m_inherited) {
			return m_inherited->isMultiPart();
		}
		return false;
	}

	void Object::setMultiPart(bool part) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_multiPart = part;
	}

	void Object::addMultiPart(Object* obj) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_multiParts.insert(obj);
	}

	std::set<Object*> Object::getMultiParts() const {
		if (m_multiProperty) {
			return m_multiProperty->m_multiParts;
		}
		if (m_inherited) {
			return m_inherited->getMultiParts();
		}
		return std::set<Object*>();
	}

	void Object::removeMultiPart(Object* obj) {
		if (!m_multiProperty) {
			return;
		}
		m_multiProperty->m_multiParts.erase(obj);

	}

	void Object::removeMultiParts() {
		if (!m_multiProperty) {
			return;
		}
		m_multiProperty->m_multiParts.clear();
	}

	void Object::addMultiPartCoordinate(int32_t rotation, ModelCoordinate coord) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_multiPartCoordinates.insert(std::pair<int32_t, ModelCoordinate>(rotation, coord));
		m_multiProperty->m_partAngleMap[rotation] = rotation;
	}

	std::multimap<int32_t, ModelCoordinate> Object::getMultiPartCoordinates() const {
		if (m_multiProperty) {
			return m_multiProperty->m_multiPartCoordinates;
		}
		if (m_inherited) {
			return m_inherited->getMultiPartCoordinates();
		}
		return std::multimap<int32_t, ModelCoordinate>();
	}

	std::vector<ModelCoordinate> Object::getMultiPartCoordinates(int32_t rotation) const {
		std::vector<ModelCoordinate> coordinates;
		
		if (m_multiProperty) {
			int32_t closest = 0;
			getIndexByAngle(rotation, m_multiProperty->m_partAngleMap, closest);
			std::pair<std::multimap<int32_t, ModelCoordinate>::iterator,
				std::multimap<int32_t, ModelCoordinate>::iterator> result = m_multiProperty->m_multiPartCoordinates.equal_range(closest);
			std::multimap<int32_t, ModelCoordinate>::iterator it = result.first;
			for (; it != result.second; ++it) {
				coordinates.push_back((*it).second);
			}
		} else if (m_inherited) {
			return m_inherited->getMultiPartCoordinates(rotation);
		}
		return coordinates;
	}

	std::vector<ModelCoordinate> Object::getMultiObjectCoordinates(int32_t rotation) const {
		std::vector<ModelCoordinate> coordinates;
		if (m_multiProperty) {
			if (m_multiProperty->m_multiObjectCoordinates.empty()) {
				std::set<Object*>::iterator subit = m_multiProperty->m_multiParts.begin();
				for (; subit != m_multiProperty->m_multiParts.end(); ++subit) {
					const std::multimap<int32_t, ModelCoordinate>& subcoords = (*subit)->getMultiPartCoordinates();
					m_multiProperty->m_multiObjectCoordinates.insert(subcoords.begin(), subcoords.end());
				}
				std::multimap<int32_t, ModelCoordinate>::iterator it = m_multiProperty->m_multiObjectCoordinates.begin();
				for (; it != m_multiProperty->m_multiObjectCoordinates.end(); ++it) {
					m_multiProperty->m_multiAngleMap[(*it).first] = (*it).first;
				}
			}
			int32_t closest = 0;
			getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
			std::pair<std::multimap<int32_t, ModelCoordinate>::iterator,
				std::multimap<int32_t, ModelCoordinate>::iterator> result = m_multiProperty->m_multiObjectCoordinates.equal_range(closest);
			std::multimap<int32_t, ModelCoordinate>::iterator it = result.first;
			ModelCoordinate parent(0,0);
			coordinates.push_back(parent);
			for (; it != result.second; ++it) {
				coordinates.push_back((*it).second);
			}
		} else if (m_inherited) {
			return m_inherited->getMultiObjectCoordinates(rotation);
		}
		return coordinates;
	}

	void Object::setRotationAnchor(const ExactModelCoordinate& anchor) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_rotationAnchor = anchor;
	}

	ExactModelCoordinate Object::getRotationAnchor() const {
		if (m_multiProperty) {
			return m_multiProperty->m_rotationAnchor;
		}
		if (m_inherited) {
			return m_inherited->getRotationAnchor();
		}
		return ExactModelCoordinate();
	}
	
	void Object::setRestrictedRotation(bool restrict) {
		if (!m_multiProperty) {
			m_multiProperty = new MultiObjectProperty();
		}
		m_multiProperty->m_restrictedRotation = restrict;
	}

	bool Object::isRestrictedRotation() const {
		if (m_multiProperty) {
			return m_multiProperty->m_restrictedRotation;
		}
		if (m_inherited) {
			return m_inherited->isRestrictedRotation();
		}
		return false;
	}

	int32_t Object::getRestrictedRotation(int32_t rotation) {
		int32_t closest = rotation;
		if (m_multiProperty) {
			if (!m_multiProperty->m_multiAngleMap.empty()) {
				getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
			} else if (!m_multiProperty->m_partAngleMap.empty()) {
				getIndexByAngle(rotation, m_multiProperty->m_partAngleMap, closest);
			}
		} else if (m_inherited) {
			return m_inherited->getRestrictedRotation(rotation);
		}
		return closest;
	}

	void Object::setZStepRange(int32_t zRange) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_zRange = zRange;
	}

	int32_t Object::getZStepRange() const {
		if (m_moveProperty) {
			return m_moveProperty->m_zRange;
		}
		if (m_inherited) {
			return m_inherited->getZStepRange();
		}
		return 0;
	}

	void Object::setArea(const std::string& id) {
		if (!m_basicProperty) {
			m_basicProperty = new BasicObjectProperty();
		}
		m_basicProperty->m_area = id;
	}

	std::string Object::getArea() const {
		if (m_basicProperty) {
			return m_basicProperty->m_area;
		}
		if (m_inherited) {
			return m_inherited->getArea();
		}
		return "";
	}

	void Object::addWalkableArea(const std::string& id) {
		if (!m_moveProperty) {
			m_moveProperty = new MovableObjectProperty();
		}
		m_moveProperty->m_walkableAreas.push_back(id);
		m_moveProperty->m_walkableAreas.sort();
		m_moveProperty->m_walkableAreas.unique();
	}

	void Object::removeWalkableArea(const std::string& id) {
		if (!m_moveProperty) {
			return;
		}
		m_moveProperty->	m_walkableAreas.remove(id);
	}

	std::list<std::string> Object::getWalkableAreas() const {
		if (m_moveProperty) {
			return m_moveProperty->m_walkableAreas;
		}
		if (m_inherited) {
			return m_inherited->getWalkableAreas();
		}
		return std::list<std::string>();
	}

	bool Object::operator==(const Object& obj) const {
		return m_id == obj.getId() && m_namespace == obj.getNamespace();
	}

	bool Object::operator!=(const Object& obj) const {
		return m_id != obj.getId() || m_namespace != obj.getNamespace();
	}

}
