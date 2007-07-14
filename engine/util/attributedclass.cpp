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
#include "util/attributedclass.h"

namespace FIFE {

	AttributedClass::AttributedClass(const std::string& className) 
		: m_className(className), m_mutex(SDL_CreateMutex()) { 
	}

	AttributedClass::AttributedClass(const AttributedClass& ac)
		: m_className(ac.m_className), m_mutex(SDL_CreateMutex()) {
		updateAttributes(&ac,true);
	}

	AttributedClass::~AttributedClass() {
		SDL_DestroyMutex(m_mutex);
	};

	void AttributedClass::updateAttributes(const AttributedClass* attrObject, bool override) {
		if( attrObject == 0 )
			return;
		SDL_mutexP(m_mutex);
		type_attributes::const_iterator i(attrObject->m_attributes.begin());
		type_attributes::const_iterator end(attrObject->m_attributes.end());
		for(; i != end; ++i) {
			if( !override && m_attributes.find(i->first) != m_attributes.end() )
				continue;
			m_attributes[ i->first ] = i->second;
		}
		SDL_mutexV(m_mutex);
	}
}; //FIFE

