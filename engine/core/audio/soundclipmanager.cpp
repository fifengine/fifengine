/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
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
#include "util/resource/resourcemanager.h"
#include "util/resource/resource.h"

#include "soundclipmanager.h"

namespace FIFE {
	static Logger _log(LM_RESMGR);

	SoundClipManager::~SoundClipManager() {

	}

	size_t SoundClipManager::getMemoryUsed() const {
		size_t totalSize = 0;

		SoundClipHandleMapConstIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		for ( ; it != itend; ++it) {
			totalSize += it->second->getSize();
		}

		return totalSize;
	}

	size_t SoundClipManager::getTotalResourcesCreated() const {
		SoundClipHandleMapConstIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_NOT_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t SoundClipManager::getTotalResourcesLoaded() const {
		SoundClipHandleMapConstIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t SoundClipManager::getTotalResources() const {
		return m_sclipHandleMap.size();
	}

	SoundClipPtr SoundClipManager::create(IResourceLoader* loader){
		SoundClip* ptr = new SoundClip(loader);
		return add(ptr);
	}

	SoundClipPtr SoundClipManager::create(const std::string& name, IResourceLoader* loader){
		if (exists(name)) {
			FL_WARN(_log, LMsg("SoundClipManager::create(std::string, IResourceLoader* loader) - ") << "Resource name " << name << " was previously created.  Returning original SoundClip...");
			return get(name);
		}

		SoundClip* ptr = new SoundClip(name, loader);
		return add(ptr);
	}

	SoundClipPtr SoundClipManager::load(const std::string& name, IResourceLoader* loader) {
		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);

		if (nit != m_sclipNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_NOT_LOADED ) {
				nit->second->load();
			}

			return nit->second;
		}

		//was not found so create and load resource
		SoundClipPtr ptr = create(name, loader);
		ptr->load();

		if (ptr->getState() == IResource::RES_NOT_LOADED){
			FL_WARN(_log, LMsg("SoundClipManager::load(std::string) - ") << "Resource name " << name << " was not found and could not be loaded.");
			remove(name);
		}

		return ptr;
	}

	SoundClipPtr SoundClipManager::add(SoundClip* res) {
		assert(res);
		assert(!(exists(res->getHandle()) || exists(res->getName())));

		SoundClipPtr resptr(res);

		std::pair<SoundClipHandleMapIterator, bool> returnValue;
		returnValue = m_sclipHandleMap.insert ( SoundClipHandleMapPair(res->getHandle(), resptr));

		if (returnValue.second) {
			m_sclipNameMap.insert ( SoundClipNameMapPair(returnValue.first->second->getName(), returnValue.first->second) );
		}
		else {
			FL_WARN(_log, LMsg("SoundClipManager::add(IResource*) - ") << "Resource " << res->getName() << " already exists.... ignoring.");
		}

		return returnValue.first->second;
	}

	bool SoundClipManager::exists(const std::string& name) {
		SoundClipNameMapIterator it = m_sclipNameMap.find(name);
		if (it != m_sclipNameMap.end()) {
			return true;
		}

		return false;
	}

	bool SoundClipManager::exists(ResourceHandle handle) {
		SoundClipHandleMapConstIterator it = m_sclipHandleMap.find(handle);
		if (it != m_sclipHandleMap.end()) {
			return true;
		}

		return false;
	}

	void SoundClipManager::reload(const std::string& name) {
		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);

		if (nit != m_sclipNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			nit->second->load();
			return;
		}

		FL_WARN(_log, LMsg("SoundClipManager::reload(std::string) - ") << "Resource name " << name << " not found.");
	}

	void SoundClipManager::reload(ResourceHandle handle) {
		SoundClipHandleMapIterator it = m_sclipHandleMap.find(handle);

		if ( it != m_sclipHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
			return;
		}

		FL_WARN(_log, LMsg("SoundClipManager::reload(ResourceHandle) - ") << "Resource handle " << handle << " not found.");

	}

	void SoundClipManager::reloadAll() {
		SoundClipHandleMapIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
		}
	}

	void SoundClipManager::loadUnreferenced() {
		SoundClipHandleMapIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED){
				it->second->load();
				count++;
			}
		}
		FL_DBG(_log, LMsg("SoundClipManager::loadUnreferenced() - ") << "Loaded " << count << " unreferenced resources.");
	}

	void SoundClipManager::free(const std::string& name) {
		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);

		if (nit != m_sclipNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("SoundClipManager::free(std::string) - ") << "Resource name " << name << " not found.");
	}

	void SoundClipManager::free(ResourceHandle handle) {
		SoundClipHandleMapConstIterator it = m_sclipHandleMap.find(handle);
		if (it != m_sclipHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("SoundClipManager::free(ResourceHandle) - ") << "Resource handle " << handle << " not found.");
	}

	void SoundClipManager::freeAll() {
		SoundClipHandleMapIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		int32_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("SoundClipManager::freeAll() - ") << "Freed all " << count << " resources.");
	}

	void SoundClipManager::freeUnreferenced() {
		SoundClipHandleMapIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED ){
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("SoundClipManager::freeUnreferenced() - ") << "Freed " << count << " unreferenced resources.");
	}

	void SoundClipManager::remove(SoundClipPtr& resource) {
		SoundClipHandleMapIterator it = m_sclipHandleMap.find(resource->getHandle());
		SoundClipNameMapIterator nit = m_sclipNameMap.find(resource->getName());

		if (it != m_sclipHandleMap.end()) {
			m_sclipHandleMap.erase(it);

			if (nit != m_sclipNameMap.end()) {
				m_sclipNameMap.erase(nit);
				return;
			}
			assert(false); //should never get here
		}

		FL_WARN(_log, LMsg("SoundClipManager::remove(ResourcePtr&) - ") << "Resource " << resource->getName() << " was not found.");
	}

	void SoundClipManager::remove(const std::string& name) {
		std::size_t handle;

		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);
		if (nit != m_sclipNameMap.end()) {
			handle = nit->second->getHandle();
			m_sclipNameMap.erase(nit);
		}
		else {
			FL_WARN(_log, LMsg("SoundClipManager::remove(std::string) - ") << "Resource " << name << " was not found.");
			return;
		}

		SoundClipHandleMapIterator it = m_sclipHandleMap.find(handle);
		if ( it != m_sclipHandleMap.end()) {
			m_sclipHandleMap.erase(it);
			return;
		}

		assert(false);  //should never get here
	}

	void SoundClipManager::remove(ResourceHandle handle) {
		std::string name;

		SoundClipHandleMapIterator it = m_sclipHandleMap.find(handle);

		if (it != m_sclipHandleMap.end()) {
			name = it->second->getName();
			m_sclipHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("SoundClipManager::remove(ResourceHandle) - ") << "Resource handle " << handle << " was not found.");
			return;
		}

		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);
		if ( nit != m_sclipNameMap.end() ) {
			m_sclipNameMap.erase(nit);
			return;
		}

		assert(false);  //should never get here
	}

	void SoundClipManager::removeAll() {
		//should always be equal
		assert (m_sclipHandleMap.size() == m_sclipNameMap.size());

		size_t count = m_sclipHandleMap.size();

		m_sclipHandleMap.clear();
		m_sclipNameMap.clear();

		FL_DBG(_log, LMsg("SoundClipManager::removeAll() - ") << "Removed all " << count << " resources.");
	}

	void SoundClipManager::removeUnreferenced() {
		SoundClipHandleMapIterator it = m_sclipHandleMap.begin(),
			itend = m_sclipHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if ( it->second.useCount() == 2) {
				remove(it->second->getHandle());
				count++;
			}
		}

		FL_DBG(_log, LMsg("SoundClipManager::removeUnreferenced() - ") << "Removed " << count << " unreferenced resources.");
	}

	SoundClipPtr SoundClipManager::get(const std::string& name) {
		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);

		if (nit != m_sclipNameMap.end()) {
			if (nit->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				nit->second->load();
			}
			return nit->second;
		}

		//not found so attempt to create and load the resource
		SoundClipPtr ptr = load(name);
		return ptr;
	}

	SoundClipPtr SoundClipManager::get(ResourceHandle handle) {
		SoundClipHandleMapConstIterator it = m_sclipHandleMap.find(handle);
		if (it != m_sclipHandleMap.end()) {
			if (it->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				it->second->load();
			}
			return it->second;
		}

		FL_WARN(_log, LMsg("SoundClipManager::get(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return SoundClipPtr();
	}

	ResourceHandle SoundClipManager::getResourceHandle(const std::string& name) {
		SoundClipNameMapIterator nit = m_sclipNameMap.find(name);
		if (nit != m_sclipNameMap.end()) {
			return nit->second->getHandle();
		}

		FL_WARN(_log, LMsg("SoundClipManager::getResourceHandle(std::string) - ") << "Resource " << name << " is undefined.");

		return 0;
	}

} //FIFE
