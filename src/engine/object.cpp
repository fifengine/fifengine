/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "object.h"
#include "imagecache.h"
#include "video/renderable.h"

#include <SDL_video.h>

namespace FIFE {

	Object::Object(const std::string& name, int position) : m_opacity( 255 ), m_name(name), m_visual(0) {
		init();
		setPosition( position );
	}

	Object::~Object() {
		cleanup();
	}

	Object::Object(const Object&) : m_name() {
		init();
		setPosition( POS_NOT_ON_MAP );
	}

	void Object::init() {}
	;


	Object& Object::operator=(const Object&) {
		return *this;
	}

	void Object::cleanup() {
		type_inventory::const_iterator end = m_inventory.end();
		for (type_inventory::iterator i = m_inventory.begin(); i != end; ++i) {
			delete *i;
		}
		m_inventory.clear();
	}

	void Object::setPosition(int pos) {
		m_position = pos;
	}

	void Object::addToInventory(Object* obj) {
		m_inventory.push_back(obj);
	};


	const std::string& Object::getName() const {
		return m_name;
	}

	size_t Object::getVisual() {
		return m_visual;
	}

	void Object::setVisual(size_t ra) {
		m_visual = ra;
	}

	int Object::getPosition() const {
		return m_position;
	}

}

//FIFE
