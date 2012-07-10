/***************************************************************************
 *   Copyright (C) 2006-2011 by the FIFE team                              *
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
	Object::Object(const std::string& identifier, const std::string& name_space, Object* inherited):
		m_id(identifier),
		m_namespace(name_space),
		m_filename(""),
		m_inherited(inherited),
		m_actions(NULL),
		m_blocking(false),
		m_static(false),
		m_pather(NULL),
		m_visual(NULL),
		m_defaultAction(NULL),
		m_cellStack(0),
		m_costId(""),
		m_cost(1.0),
		m_multiPart(false),
		m_restrictedRotation(false) {
	}

	Object::~Object() {
		if (m_actions) {
			std::map<std::string, Action*>::const_iterator i(m_actions->begin());
			while (i != m_actions->end()) {
				delete i->second;
				++i;
			}
			delete m_actions;
		}
		delete m_visual;
	}

	Action* Object::createAction(const std::string& identifier, bool is_default) {
		if (!m_actions) {
			m_actions = new std::map<std::string, Action*>;
		}

		std::map<std::string, Action*>::const_iterator it = m_actions->begin();
		for(; it != m_actions->end(); ++it) {
			if(identifier == it->second->getId()) {
				throw NameClash(identifier);
			}
		}

		Action* a = getAction(identifier);
		if (!a) {
			a = new Action(identifier);
			(*m_actions)[identifier] = a;
			if (is_default || (!m_defaultAction)) {
				m_defaultAction = a;
			}
		}
		return a;
	}

	Action* Object::getAction(const std::string& identifier) const {
		std::map<std::string, Action*>::const_iterator i;
		if (m_actions) {
			i = m_actions->find(identifier);
		}
		if ((!m_actions) || (i == m_actions->end())) {
			if (m_inherited) {
				return m_inherited->getAction(identifier);
			}
			return NULL;
		}
		return i->second;
	}

	std::list<std::string> Object::getActionIds() const {
		std::list<std::string> action_ids;
		action_ids.clear();
		if (m_actions) {
			std::map<std::string, Action*>::const_iterator actions_it = m_actions->begin();
			for(; actions_it != m_actions->end(); ++actions_it) {
				action_ids.push_back(actions_it->first);
			}
		}
		return action_ids;
	}

	void Object::setDefaultAction(const std::string& identifier) {
		std::map<std::string, Action*>::const_iterator i;
		Action* action = NULL;
		if (m_actions) {
			i = m_actions->find(identifier);
		}
		if ((!m_actions) || (i == m_actions->end())) {
			if (m_inherited) {
				action = m_inherited->getAction(identifier);
			}
		} else {
			action = i->second;
		}

		if (action) {
			m_defaultAction = action;
		}
	}

	void Object::setPather(IPather* pather) {
		m_pather = pather;
	}

	bool Object::isBlocking() const {
		if (m_blocking) {
			return true;
		}
		if (m_inherited) {
			return m_inherited->isBlocking();
		}
		return false;
	}

	bool Object::isStatic() const {
		if (m_static) {
			return true;
		}
		if (m_inherited) {
			return m_inherited->isStatic();
		}
		return false;
	}

	void Object::setFilename(const std::string& file) {
		m_filename = file;
	}

	const std::string& Object::getFilename() const {
		return m_filename;
	}

	void Object::setCellStackPosition(uint8_t position) {
		m_cellStack = position;
	}

	uint8_t Object::getCellStackPosition() {
		return m_cellStack;
	}

	bool Object::isSpecialCost() {
		return m_costId != "";
	}

	void Object::setCostId(const std::string& cost) {
		m_costId = cost;
	}

	const std::string& Object::getCostId() const {
		return m_costId;
	}

	void Object::setCost(double cost) {
		m_cost = cost;
	}
	
	double Object::getCost() const {
		return m_cost;
	}

	bool Object::isMultiObject() {
		return !m_multiPartIds.empty();
	}

	void Object::addMultiPartId(const std::string& partId) {
		m_multiPartIds.push_back(partId);
	}

	const std::list<std::string>& Object::getMultiPartIds() {
		return m_multiPartIds;
	}

	void Object::removeMultiPartId(const std::string& partId) {
		std::list<std::string>::iterator it = m_multiPartIds.begin();
		for (; it != m_multiPartIds.end(); ++it) {
			if (*it == partId) {
				m_multiPartIds.erase(it);
				break;
			}
		}
	}
	
	void Object::removeAllMultiPartIds() {
		m_multiPartIds.clear();
	}

	bool Object::isMultiPart() {
		return m_multiPart;
	}

	void Object::setMultiPart(bool part) {
		m_multiPart = part;
	}

	void Object::addMultiPart(Object* obj) {
		m_multiParts.insert(obj);
	}

	const std::set<Object*>& Object::getMultiParts() {
		return m_multiParts;
	}

	void Object::removeMultiPart(Object* obj) {
		m_multiParts.erase(obj);

	}

	void Object::removeMultiParts() {
		m_multiParts.clear();
	}

	void Object::addMultiPartCoordinate(int32_t rotation, ModelCoordinate coord) {
		m_multiPartCoordinates.insert(std::pair<int32_t, ModelCoordinate>(rotation, coord));
		m_partAngleMap[rotation] = rotation;
	}

	const std::multimap<int32_t, ModelCoordinate>& Object::getMultiPartCoordinates() {
		return m_multiPartCoordinates;
	}

	std::vector<ModelCoordinate> Object::getMultiPartCoordinates(int32_t rotation) {
		std::vector<ModelCoordinate> coordinates;
		int32_t closest = 0;
		int32_t index = getIndexByAngle(rotation, m_partAngleMap, closest);
		std::pair<std::multimap<int32_t, ModelCoordinate>::iterator,
			std::multimap<int32_t, ModelCoordinate>::iterator> result = m_multiPartCoordinates.equal_range(closest);
		std::multimap<int32_t, ModelCoordinate>::iterator it = result.first;
		for (; it != result.second; ++it) {
			coordinates.push_back((*it).second);
		}
		return coordinates;
	}

	std::vector<ModelCoordinate> Object::getMultiObjectCoordinates(int32_t rotation) {
		if (m_multiObjectCoordinates.empty()) {
			std::set<Object*>::iterator subit = m_multiParts.begin();
			for (; subit != m_multiParts.end(); ++subit) {
				const std::multimap<int32_t, ModelCoordinate>& subcoords = (*subit)->getMultiPartCoordinates();
				m_multiObjectCoordinates.insert(subcoords.begin(), subcoords.end());
			}
			std::multimap<int32_t, ModelCoordinate>::iterator it = m_multiObjectCoordinates.begin();
			for (; it != m_multiObjectCoordinates.end(); ++it) {
				m_multiAngleMap[(*it).first] = (*it).first;
			}
		}
		int32_t closest = 0;
		int32_t index = getIndexByAngle(rotation, m_multiAngleMap, closest);
		std::vector<ModelCoordinate> coordinates;
		std::pair<std::multimap<int32_t, ModelCoordinate>::iterator,
			std::multimap<int32_t, ModelCoordinate>::iterator> result = m_multiObjectCoordinates.equal_range(closest);
		std::multimap<int32_t, ModelCoordinate>::iterator it = result.first;
		ModelCoordinate parent(0,0);
		coordinates.push_back(parent);
		for (; it != result.second; ++it) {
			coordinates.push_back((*it).second);
		}
		return coordinates;
	}

	void Object::setRotationAnchor(const ExactModelCoordinate& anchor) {
		m_rotationAnchor = anchor;
	}

	const ExactModelCoordinate& Object::getRotationAnchor() {
		return m_rotationAnchor;
	}
	
	void Object::setRestrictedRotation(bool restrict) {
		m_restrictedRotation = restrict;
	}

	bool Object::isRestrictedRotation() {
		return m_restrictedRotation;
	}

	int32_t Object::getRestrictedRotation(int32_t rotation) {
		int32_t closest = 0;
		if (!m_multiAngleMap.empty()) {
			int32_t index = getIndexByAngle(rotation, m_multiAngleMap, closest);
		} else if (!m_partAngleMap.empty()) {
			int32_t index = getIndexByAngle(rotation, m_partAngleMap, closest);
		}
		return closest;
	}

	bool Object::operator==(const Object& obj) const {
		return m_id == obj.getId() && m_namespace == obj.getNamespace();
	}

	bool Object::operator!=(const Object& obj) const {
		return m_id != obj.getId() || m_namespace != obj.getNamespace();
	}

}
