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

#ifndef FIFE_RESOURE_PTR_H
#define FIFE_RESOURE_PTR_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resource.h"
#include "pool.h"

namespace FIFE {

	/** A lazy loading resource.
	 */
	class ResourcePtr {
	public:
		ResourcePtr(IResource* ptr = 0) : m_ptr(ptr),m_pool(0),m_index(Pool::INVALID_ID) {
			if( m_ptr )
				m_ptr->addRef();
		}

		ResourcePtr(Pool* pool,int index) : m_ptr(0),m_pool(pool),m_index(index) {
		}

		ResourcePtr(const ResourcePtr& r)
		: m_ptr(r.m_ptr),m_pool(r.m_pool),m_index(r.m_index) {
			if(m_ptr) {
				 m_ptr->addRef();
			}
		}

		ResourcePtr& operator=(const ResourcePtr& r) {
			if( this == &r )
				return *this;
			release();
			m_ptr = r.m_ptr;
			m_pool = r.m_pool;
			m_index = r.m_index;
			if(m_ptr) {
				 m_ptr->addRef();
			}
			return *this;
		}

		~ResourcePtr() {
			release();
		}

		IResource& operator->() {
			load();
			return *m_ptr;
		}

		const IResource& operator->() const {
			constLoad();
			return *m_ptr;
		}

		operator bool() const {
			return isValid();
		}

		bool isValid() const {
			return m_ptr || isLoadable();
		}

		bool isLoadable() const {
			return m_pool && m_index != Pool::INVALID_ID;
		}

		bool operator==(const ResourcePtr& r) const {
			if( isLoadable() && r.isLoadable() )
				return m_index == r.m_index && m_pool == r.m_pool;
			if( !isLoadable() && !r.isLoadable() )
				return m_ptr == r.m_ptr;
			return false;
		}

		bool operator<(const ResourcePtr& r) const {
			if( isLoadable() && r.isLoadable() )
			{
				if( m_pool == r.m_pool )
					return m_index < r.m_index;
				return m_pool < r.m_pool;
			}
			if( !isLoadable() && !r.isLoadable() )
				return m_ptr < r.m_ptr;
			return isLoadable() < r.isLoadable();
		}

		void release() {
			if(m_ptr) {
				 m_ptr->decRef();
			}
			m_ptr = 0;
		}

		void load() {
			constLoad();
		}

		void unload() {
			release();
			if( isLoadable() ) {
				m_pool->release(m_index);
			}
		}

		template<class T>
		T* get() {
			if( isValid() )
				load();
			return dynamic_cast<T*>(m_ptr);
		}

	private:
		void constLoad() const {
			if( m_ptr )
				return;
			m_ptr = &m_pool->get(m_index);
			m_ptr->addRef();
		}
		mutable IResource* m_ptr;
		Pool* m_pool;
		int m_index;
	};
}
#endif

