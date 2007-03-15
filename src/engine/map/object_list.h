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

#ifndef FIFE_MAP_OBJECTLIST_H
#define FIFE_MAP_OBJECTLIST_H

// Standard C++ library includes
#include <deque>
#include <map>

// 3rd party library includes
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder


namespace FIFE { namespace map {

	class ObjectInfo;
	typedef boost::shared_ptr<ObjectInfo> ObjectPtr;

	/** 
	 */
	class ObjectList {
		public:
			typedef boost::function1<void,ObjectPtr> type_callback;

			ObjectList(){};
			ObjectList(const ObjectList&);
			~ObjectList();

			void append(ObjectPtr entry);
			void insert(ObjectPtr before, ObjectPtr entry);
			void erase(ObjectPtr entry);
			bool contains(ObjectPtr entry) const;
			ObjectPtr at(size_t index) const;
			size_t size() const;
			bool empty() const;
			void clear();

			void setRemoveCallback(const type_callback& callback);
			void setInsertCallback(const type_callback& callback);

		protected:
			typedef std::deque<ObjectPtr> type_list;
			typedef type_list::iterator iterator;
			typedef std::map<ObjectPtr,iterator> type_map;

			type_list m_list;
			type_map  m_map;
			type_callback m_on_remove;
			type_callback m_on_insert;
	};



} } //FIFE::map

#endif
/* vim: set noexpandtab: set shiftwidth=2: set tabstop=2: */
