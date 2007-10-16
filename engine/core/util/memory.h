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
#ifndef FIFE_UTIL_MEMORY_H
#define FIFE_UTIL_MEMORY_H

// Standard C++ library includes
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "exception.h"

namespace FIFE {
	class IManagedItem;
	
	class IMemoryManager {
	public:
		virtual ~IMemoryManager() {}
		
		/** Adds new item underneath memory management of this manager
		 * @param item item to add 
		 */
		virtual void addManagedItem(IManagedItem* item) = 0;
		
		/** Removes item underneath memory management of this manager
		 * @param item item to remove
		 */
		virtual void removeManagedItem(IManagedItem* item) = 0;
	};

	class IManagedItem {
	public:
		virtual ~IManagedItem() {}
		
		/** Returns the memory manager assigned for this item
		 * @return manager managing this item, NULL if none
		 */
		virtual IMemoryManager* getMemoryManager() = 0;
	};
	
	template<typename T>
	inline void addMemoryManagedItem(std::vector<T*>& container, IManagedItem* item) {
		if (item->getMemoryManager()) {
			throw Duplicate("Tried to add already managed item to container");
		}
		T* downcast_item = dynamic_cast<T*>(item);
		if (!downcast_item) {
			throw InvalidConversion("Tried to add managed item with incorrect type");
		}
		container.push_back(downcast_item);
	}
	
	template<typename T>
	inline void removeMemoryManagedItem(std::vector<T*>& container, IManagedItem* item) {
		typename std::vector<T*>::iterator it = container.begin();
		for (; it != container.end(); ++it) {
			if(*it == item) {
				container.erase(it);
				return;
			}
		}
	}
	
	template<typename T>
	inline void purgeMemoryManagedItems(std::vector<T*>& container) {
		typename std::vector<T*>::iterator i;
		for (i = container.begin(); i != container.end(); i++) {
			delete *i;
			*i = 0;
		}
		container.clear();
	}
}

#endif
