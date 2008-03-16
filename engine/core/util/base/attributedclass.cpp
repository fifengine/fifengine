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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/attributedclass.h"

namespace FIFE {

	AttributedClass::AttributedClass(const std::string& identifier) 
		: ResourceClass(), m_id(identifier) { 
	}

	AttributedClass::~AttributedClass() {
	}

	const std::string& AttributedClass::Id() const {
		return m_id;
	}

	std::vector<std::string> AttributedClass::listFields() const {
		std::vector<std::string> list;

		std::map<std::string,std::string>::const_iterator i = m_fields.begin();
		for(; i != m_fields.end(); ++i) {
			list.push_back(i->first);
		}

		return list;
	}

	void AttributedClass::set(const std::string& field, const std::string& value) {
		if(field == "id") {
			m_id = value;
			return ;
		}
		m_fields[field] = value;
	}

	const std::string& AttributedClass::get(const std::string& field) {
		static std::string null = "";
		
		if(field == "id")
			return m_id;

		if(m_fields.find(field) == m_fields.end()) {
			return null;
		}

		return m_fields[field];
	}

	void AttributedClass::remove(const std::string& field) {
		m_fields.erase(field);
	}

}; //FIFE

