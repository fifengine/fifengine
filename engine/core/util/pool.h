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

#ifndef FIFE_POOL_H
#define FIFE_POOL_H

// Standard C++ library includes
#include <map>
#include <vector>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {


	class PoolListener {
	public:
		virtual void poolCleared() = 0;
		virtual ~PoolListener() {};
	};

	/**  Pool is used to optimize memory usage for objects
	 *
	 * Pool guarantees that there is minimal amount of resources
	 *   used in cases when it is would possible that multiple 
	 *   instances of the same data would be loaded into the memory.
	 */
	template <typename T> class Pool {
	public:
		/** Default constructor.
		 */
		Pool();

		/** Destructor.
		 */
		virtual ~Pool();

		/** Adds new object into the pool, transfers the ownership to the pool
		 */
		virtual int add(T* obj);

		/** Gets object from pool with given index
		 */
		virtual T& get(int index) const;

		/** Clears pool from objects. Free's memory associated with 
		 */
		virtual void clear();

		/** Adds pool listener.
		 * Pool listeners get indications e.g. when ownerships of pooled
		 * objects change.
		 */
		virtual void addPoolListener(PoolListener* listener);

		/** Removes pool listener
		 */
		virtual void removePoolListener(PoolListener* listener);

	protected:
	private:
		std::map<int, T*> m_pooledobjs;
		std::vector<PoolListener*> m_listeners;
		int m_curind;
	};

} // FIFE

#endif
