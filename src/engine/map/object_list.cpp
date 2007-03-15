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

#include "object_list.h"
#include "objectinfo.h"

namespace FIFE { namespace map {

	ObjectList::ObjectList(const ObjectList& slist) {
		for(size_t i=0; i!=slist.size(); ++i) {
			append( slist.at(i) );
		}
	}

	ObjectList::~ObjectList() {
		clear();
	}

	bool ObjectList::contains(ObjectPtr entry) const {
		return m_map.find(entry) != m_map.end();
	}
	
	void ObjectList::append(ObjectPtr entry) {
		if( !contains(entry) ) {
			m_list.push_back( entry );
			m_map.insert( std::make_pair(entry,--m_list.end()) );
			if( m_on_insert ) {
				m_on_insert( entry );
			}
		}
	}

	void ObjectList::insert(ObjectPtr before, ObjectPtr entry) {
		if( !contains(entry) ) {
			type_map::iterator i(m_map.find(before));
			m_list.insert(i->second,entry);
			type_list::iterator list_i = i->second;
			--list_i;
			m_map.insert( std::make_pair(entry, list_i) );
			if( m_on_insert ) {
				m_on_insert( entry );
			}
		}
	}

	void ObjectList::erase(ObjectPtr entry) {
		type_map::iterator i(m_map.find(entry));
		if( i == m_map.end() ) {
			return;
		}
		m_list.erase( i->second );
		m_map.erase( i );
		if( m_on_remove ) {
			m_on_remove( entry );
		} else {
			entry->resetOwner();
		}
	}

	ObjectPtr ObjectList::at(size_t index) const {
		return m_list.at(index);
	}

	void ObjectList::clear() {
		m_map.clear();
		for(type_list::iterator i = m_list.begin(); i != m_list.end(); ++i) {
			if( m_on_remove ) {
				m_on_remove( *i );
			} else {
				(*i)->resetOwner();
			}
		}
		m_list.clear();
	}

	size_t ObjectList::size() const {
		assert( m_map.size() == m_list.size() );
		return m_map.size();
	}

	bool ObjectList::empty() const {
		return m_list.empty();
	}

	void ObjectList::setRemoveCallback(const type_callback& callback) {
		m_on_remove = callback;
	}

	void ObjectList::setInsertCallback(const type_callback& callback) {
		m_on_insert = callback;
	}

}}

