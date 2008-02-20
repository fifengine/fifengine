/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include "util/exception.h"
#include "util/logger.h"

#include "pool.h"

namespace FIFE {
	static Logger _log(LM_POOL);
	
	Pool::Pool(): 
		FifeClass(),
		m_entries(),
		m_listeners(),
		m_loaders(),
		m_curind(0)
	{
	}

	Pool::~Pool() {
		clear();
		std::vector<IPooledResourceLoader*>::iterator loader;
		for (loader = m_loaders.begin(); loader != m_loaders.end(); loader++) {
			delete (*loader);
		}
	}
	
	void Pool::clear() {
		std::vector<IPoolListener*>::iterator listener;
		for (listener = m_listeners.begin(); listener != m_listeners.end(); listener++) {
			(*listener)->poolCleared();
		}
		std::vector<PoolEntry*>::iterator entry;
		for (entry = m_entries.begin(); entry != m_entries.end(); entry++) {
			delete (*entry);
		}
		m_entries.clear();
	}

	void Pool::addResourceLoader(IPooledResourceLoader* loader) {
		m_loaders.push_back(loader);
	}

	int Pool::addResourceFromLocation(const ResourceLocation& loc) {
		std::vector<PoolEntry*>::iterator it = m_entries.begin();
		int index = 0;
		for (; it != m_entries.end(); it++) {
			ResourceLocation* loc2 = (*it)->location;
			if (*loc2 == loc) {
				return index;
			}
			index++;
		}
		
		PoolEntry* entry = new PoolEntry();
		entry->location = loc.clone();
		m_entries.push_back(entry);
		index = m_entries.size();
		return index - 1;
	}

	int Pool::addResourceFromFile(const std::string& filename) {
		return addResourceFromLocation(ResourceLocation(filename));
	}

	IPooledResource& Pool::get(unsigned int index, bool inc) {
		if (index >= m_entries.size()) {
			FL_ERR(_log, LMsg("Tried to get with index ") << index << ", only " << m_entries.size() << " items in pool");
			throw IndexOverflow( __FUNCTION__ );
		}
		IPooledResource* res = NULL;
		PoolEntry* entry = m_entries[index];
		if (entry->resource) {
			res = entry->resource;
		} else {
			if (!entry->loader) {
				findAndSetProvider(*entry);
			} else {
				entry->resource = entry->loader->loadPooledResource(*entry->location);
			}
			if (!entry->loader) {
				LMsg msg("No suitable loader was found for resource ");
				msg << entry->location->getFilename();
				FL_ERR(_log, msg);
				throw NotFound(msg.str);
			}
			if (!entry->resource) {
				LMsg msg("No loader was able to load the requested resource ");
				msg << entry->location->getFilename();
				FL_ERR(_log, msg);
				throw NotFound(msg.str);
			}
			res = entry->resource;
		}
		if (inc) {
			res->addRef();
		}
		return *res;
	}
	
	unsigned int Pool::getIndex(const std::string& filename) {
		std::vector<PoolEntry*>::iterator it = m_entries.begin();
		int index = 0;
		
		// look for the appropriate entry
		for (; it != m_entries.end(); it++) {
			
				if ((*it)->location->getFilename() == filename) {
					return index;
				}
			index++;
		}
		
		// create resource
		return addResourceFromFile(filename);
	}

	void Pool::release(unsigned int index, bool dec) {
		if (index >= m_entries.size()) {
			throw IndexOverflow( __FUNCTION__ );
		}

		IPooledResource* res = NULL;
		PoolEntry* entry = m_entries[index];
		if (entry->resource) {
			res = entry->resource;
			if (dec) {
				res->decRef();
			}
			if(res->getRefCount() == 0) {
				delete entry->resource;
				entry->resource = 0;
			}
		}
	}

	int Pool::getResourceCount(int status) {
		int amount = 0;
		std::vector<PoolEntry*>::iterator entry;
		for (entry = m_entries.begin(); entry != m_entries.end(); entry++) {
			if (status & RES_LOADED) {
				if ((*entry)->resource) {
					amount++;
				}
			}
			if (status & RES_NON_LOADED) {
				if (!(*entry)->resource) {
					amount++;
				}
			}
		}
		return amount;
	}

	void Pool::addPoolListener(IPoolListener* listener) {
		m_listeners.push_back(listener);
	}

	void Pool::removePoolListener(IPoolListener* listener) {
		std::vector<IPoolListener*>::iterator i = m_listeners.begin();
		while (i != m_listeners.end()) {
			if ((*i) == listener) {
				m_listeners.erase(i);
				return;
			}
			++i;
		}
	}

	void Pool::findAndSetProvider(PoolEntry& entry) {
		std::vector<IPooledResourceLoader*>::iterator it = m_loaders.begin();
		std::vector<IPooledResourceLoader*>::iterator end = m_loaders.end();
		if( it == end ) {
			FL_PANIC(_log, "no loader constructors given for resource pool");
		}
		for(; it != end; ++it) {
			try {
				entry.resource = (*it)->loadPooledResource(*entry.location);
			} catch (Exception e) {
				FL_ERR(_log, e.getMessage());
				continue;
			}

			if( !entry.resource )
				continue;

			entry.loader = *it;
			return;
		};
	}

	void Pool::printStatistics() {
		FL_LOG(_log, LMsg("Pool size =") << m_entries.size());
	}
}
