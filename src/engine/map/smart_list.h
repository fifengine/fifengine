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

#ifndef FIFE_MAP_SMARTLIST_H
#define FIFE_MAP_SMARTLIST_H

// Standard C++ library includes
#include <deque>
#include <map>

// 3rd party library includes
#include <boost/shared_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder


namespace FIFE { namespace map {

	/** 
	 */
	template<typename T>
	class SmartList {
		public:
			typedef T EntryType;
			typedef boost::shared_ptr<T> EntryPtr;

			SmartList(){};
			SmartList(const SmartList&);

			void append(EntryPtr entry);
			void insert(EntryPtr before, EntryPtr entry);
			void erase(EntryPtr entry);
			bool contains(EntryPtr entry) const;
			EntryPtr at(size_t index) const;
			size_t size() const;
			bool empty() const;
			void clear();

		protected:
			typedef std::deque<EntryPtr> type_list;
			typedef typename type_list::iterator iterator;
			typedef std::map<EntryPtr,iterator> type_map;

			type_list m_list;
			type_map  m_map;
	};

	template<typename T>
	SmartList<T>::SmartList(const SmartList<T>& slist) {
		for(size_t i=0; i!=slist.size(); ++i) {
			append( slist.at(i) );
		}
	}

	template<typename T>
	bool SmartList<T>::contains(EntryPtr entry) const {
		return m_map.find(entry) != m_map.end();
	}
	
	template<typename T>
	void SmartList<T>::append(EntryPtr entry) {
		m_list.push_back( entry );
		m_map.insert( std::make_pair(entry,--m_list.end()) );
	}

	template<typename T>
	void SmartList<T>::insert(EntryPtr before, EntryPtr entry) {
		typename type_map::iterator i(m_map.find(before));
		m_list.insert(i->second,entry);
		typename type_list::iterator list_i = i->second;
		--list_i;
		m_map.insert( std::make_pair(list_i) );
	}

	template<typename T>
	void SmartList<T>::erase(EntryPtr entry) {
		typename type_map::iterator i(m_map.find(entry));
		if( i == m_map.end() ) {
			return;
		}
		m_list.erase( i->second );
		m_map.erase( i );
	}

	template<typename T>
	typename SmartList<T>::EntryPtr SmartList<T>::at(size_t index) const {
		return m_list.at(index);
	}

	template<typename T>
	void SmartList<T>::clear() {
		m_map.clear();
		m_list.clear();
	}

	template<typename T>
	size_t SmartList<T>::size() const {
		assert( m_map.size() == m_list.size() );
		return m_map.size();
	}

	template<typename T>
	bool SmartList<T>::empty() const {
		return size() == 0;
	}


} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
