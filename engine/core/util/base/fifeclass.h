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

#ifndef FIFE_CLASS_H
#define FIFE_CLASS_H

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource.h"

namespace FIFE {

	typedef unsigned int fifeid_t;

	/** Base class for all fife classes
	 * Used e.g. to track instances over swig conversion
	 */
	class FifeClass: public virtual IReferenceCounted {
	public:
		FifeClass(): m_fifeid(m_curid++), m_refcount(0) { }
		
		virtual ~FifeClass() { assert(m_refcount == 0); }
		
		/** Gets unique id of this instance inside the engine
		 */
		fifeid_t getFifeId() { return m_fifeid; }
		
		/** Calling this method marks resource be used by some resource client.
		 *  It adds one to resource counter that is kept up by the resource itself.
		 *  When resource is about to be deleted (e.g. due to pooling algorithms), 
		 *  reference counter is inspected. In case value is non-zero, resource 
		 *  shouldn't be deleted.
		 */
		virtual void addRef() { m_refcount++; };

		/** Calling this method unmarks resource be used by a resource client.
		 *  @see addRef
		 */
		virtual void decRef() { m_refcount--; };

		/** Gets the current reference count
		 *  @see addRef
		 */
		virtual unsigned int getRefCount() { return m_refcount; };
		
	private:
		fifeid_t m_fifeid;
		static fifeid_t m_curid;
		unsigned int m_refcount;
	};
}

#endif
