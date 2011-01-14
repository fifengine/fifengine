/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

#include "resource_manager.h"
#include "resource.h"

namespace FIFE {
	static Logger _log(LM_RESMGR);

	IResourceManager::~IResourceManager() {

	}

	void IResourceManager::addChangeListener(IResourceManagerListener* listener) {
		m_changelisteners.push_back(listener);
	}

	void IResourceManager::removeChangeListener(IResourceManagerListener* listener) {
		std::vector<IResourceManagerListener*>::iterator i = m_changelisteners.begin();
		while (i != m_changelisteners.end()) {
			if ((*i) == listener) {
				m_changelisteners.erase(i);
				return;
			}
			++i;
		}
	}

	std::size_t IResourceManager::getMemoryUsed() const {
		std::size_t totalSize = 0;

		ResourceHandleMapConstIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			totalSize = it->second->getSize();
		}

		return totalSize;
	}

	ResourcePtr IResourceManager::add(IResource* res){
		assert(res);

		ResourcePtr resptr(res);
		m_resHandleMap.insert ( ResourceHandleMapPair(res->getHandle(), resptr));
		return resptr;
	}

	void IResourceManager::reload(const std::string& name) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getName() == name ) {
				it->second->free();
				it->second->load();
			}
		}
	}

	void IResourceManager::reload(ResourcePtr& res) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second == res ) {
				it->second->free();
				it->second->load();
			}
		}
	}

	void IResourceManager::reloadAll() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			it->second->free();
			it->second->load();
		}
	}

	void IResourceManager::loadUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if (it->second.useCount() == 1 ){  //count is 1 so only the map has a reference to it
				it->second->load();
			}
		}
	}

	void IResourceManager::free(const std::string& name) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getName() == name ) {
				it->second->free();
			}
		}
	}

	void IResourceManager::free(ResourceHandle handle) {
		ResourceHandleMapConstIterator it = m_resHandleMap.find(handle);
		if (it != m_resHandleMap.end()) {
			it->second->free();
		}
	}

	void IResourceManager::freeAll() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			it->second->free();
		}
	}

	void IResourceManager::freeUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		int32_t count = 0;
		for ( ; it != m_resHandleMap.end(); it++) {
			if (it->second.useCount() == 1 ){  //count is 1 so only the map has a reference to it
				it->second->free();
				count++;
			}
		}
		FL_LOG(_log, LMsg("IResourceManager::freeUnreferenced() - ") << "Freed " << count << " unreferenced resources.");
	}

	void IResourceManager::remove(ResourcePtr& resource) {
		ResourceHandleMapIterator it = m_resHandleMap.find(resource->getHandle());

		if (it != m_resHandleMap.end()) {
			m_resHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("IResourceManager::remove(ResourcePtr&) - ") << "Resource " << resource->getName() << " was not found.");
		}
	}

	void IResourceManager::remove(const std::string& name) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getName() == name ) {
				m_resHandleMap.erase(it);
				return;
			}
		}

		FL_WARN(_log, LMsg("IResourceManager::remove(std::string) - ") << "Resource " << name << " was not found.");
	}

	void IResourceManager::remove(ResourceHandle handle) {
		ResourceHandleMapIterator it = m_resHandleMap.find(handle);

		if (it != m_resHandleMap.end()) {
			m_resHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("IResourceManager::remove(ResourceHandle) - ") << "Resource handle " << handle << " was not found.");
		}
	}

	void IResourceManager::removeAll() {
		m_resHandleMap.clear();
	}

	void IResourceManager::removeUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		int32_t count = 0;
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second.useCount() == 1) {
				m_resHandleMap.erase(it);
				count++;
			}
		}

		FL_LOG(_log, LMsg("IResourceManager::removeUnreferenced() - ") << "Removed " << count << " unreferenced resources.");
	}

	ResourcePtr IResourceManager::get(const std::string& name) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getName() == name ) {
				return it->second;
			}
		}

		//not found... attempt to load
		ResourcePtr ptr = load(name);

		if (!ptr){
			FL_WARN(_log, LMsg("IResourceManager::get(std::string) - ") << "Resource name " << name << " was not found and could not be loaded.");
		}

		return ptr;
	}

	ResourcePtr IResourceManager::get(ResourceHandle handle) {
		ResourceHandleMapConstIterator it = m_resHandleMap.find(handle);
		if (it != m_resHandleMap.end()) {
			if (it->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				it->second->load();
			}
			return it->second;
		}
		FL_WARN(_log, LMsg("IResourceManager::get(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return ResourcePtr();
	}

	ResourceHandle IResourceManager::getResourceHandle(const std::string& name) {
		ResourceHandleMapIterator it = m_resHandleMap.begin();

		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getName() == name ) {
				return it->second->getHandle();
			}
		}
		FL_WARN(_log, LMsg("IResourceManager::getResourceHandle(std::string) - ") << "Resource " << name << " is undefined.");

		return 0;
	}
} //FIFE
