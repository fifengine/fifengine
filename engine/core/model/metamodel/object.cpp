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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/exception.h"

#include "object.h"
#include "action.h"
#include "abstractpather.h"

namespace FIFE {
	Object::Object(const std::string& identifier, Object* inherited):
		AttributedClass(identifier),
		m_inherited(inherited),
		m_actions(NULL),
		m_blocking(false),
		m_static(false),
		m_pather(NULL),
		m_visual(NULL) {
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

	const std::string& Object::get(const std::string& field) {
		static std::string null = "";

		if(field == "parent") {
			if(m_inherited) {
				return m_inherited->Id();
			}
			else {
				return null;
			}
		}

		const std::string& s = AttributedClass::get(field);
		if(s != "")
			return s;

		if(m_inherited)
			return m_inherited->get(field);

		return null;
	}

	Action* Object::createAction(const std::string& identifier) {
		if (!m_actions) {
			m_actions = new std::map<std::string, Action*>;
		}

		std::map<std::string, Action*>::const_iterator it = m_actions->begin();
		for(; it != m_actions->end(); ++it) {
			if(identifier == it->second->Id())
				throw NameClash("Action identifer " + identifier + " has multiple occurances in Object: " + Id() + ".");
		}

		Action* a = getAction(identifier);
		if (!a) {
			a = new Action(identifier);
			(*m_actions)[identifier] = a;
		}
		return a;
	}

	Action* Object::getAction(const std::string& identifier) {
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

	void Object::setPather(AbstractPather* pather) {
		m_pather = pather;
	}
	
	bool Object::isBlocking() {
		if (m_blocking) {
			return true;
		}
		if (m_inherited) {
			return m_inherited->isBlocking();
		}
		return false;
	}
	
	bool Object::isStatic() {
		if (m_static) {
			return true;
		}
		if (m_inherited) {
			return m_inherited->isStatic();
		}
		return false;
	}	
}
