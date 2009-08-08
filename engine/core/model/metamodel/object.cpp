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

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "object.h"
#include "action.h"
#include "abstractpather.h"

namespace FIFE {
	Object::Object(const std::string& identifier, const std::string& name_space, Object* inherited):
		m_id(identifier),
		m_namespace(name_space),
		m_inherited(inherited),
		m_actions(NULL),
		m_blocking(false),
		m_static(false),
		m_pather(NULL),
		m_visual(NULL),
		m_defaultaction(NULL) {
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
			if (is_default || (!m_defaultaction)) {
				m_defaultaction = a;
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

	void Object::setPather(AbstractPather* pather) {
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

	bool Object::operator==(const Object& obj) const {
		return m_id == obj.getId() && m_namespace == obj.getNamespace();
	}

	bool Object::operator!=(const Object& obj) const {
		return m_id != obj.getId() || m_namespace != obj.getNamespace();
	}

}
