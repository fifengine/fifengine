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
#include "objectinfo.h"
#include "objectmanager.h"

// XXX I can hardly follow this. We're building a n-ary tree of objects or
// something like that? Names like 'finalize' don't help much; we're not in
// Java here -- zahlman

namespace FIFE { namespace map {

	ObjectManager::ObjectManager() : m_map(0), m_objects() {
		m_objects.push_back(new ObjectInfo());
	}

	ObjectManager::~ObjectManager() {
		type_objects::iterator it = m_objects.begin();
		for (; it != m_objects.end(); ++it) {
			delete *it;
		}
	}

	size_t ObjectManager::addObject(ObjectInfo* moi) {
		if (!moi) { return 0; }
		finalizeObject(moi);
		m_objects.push_back(moi);
		return m_objects.size() - 1;
	}

	ObjectInfo* ObjectManager::getObject(size_t objectId) {
		return m_objects[objectId];
	}

	ObjectManager::type_objects& ObjectManager::getObjects() {
		return m_objects;
	}

	void ObjectManager::initialize(Map* m) {
		m_map = m;
		finalizeAllObjects();
	}

	bool ObjectManager::finalizeObject(ObjectInfo* moi) {
		if (!moi) {
			return false;
		}
		
		ObjectInfo::type_temporaryInventory::iterator it = moi->m_temporaryInventory.begin();
		for(; it != moi->m_temporaryInventory.end(); ++it) {
			if (finalizeObject(*it)) {
				size_t objectId = addObject(*it);
				moi->m_inventory.push_back(objectId);
			} else {
				delete *it;
			}
		}
		moi->m_temporaryInventory.clear();
		return true;
	}

	void ObjectManager::finalizeAllObjects() {
		for (type_objects::iterator it = m_objects.begin();
		     it != m_objects.end(); ++it) {
			if (!finalizeObject(*it)) {
				delete *it;
				*it = 0;
			}
		}
	}
} } // FIFE::map
