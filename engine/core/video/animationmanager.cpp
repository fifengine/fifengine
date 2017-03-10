/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
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
#include <tinyxml.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "util/resource/resourcemanager.h"
#include "util/resource/resource.h"
#include "video/renderbackend.h"

#include "animationmanager.h"

namespace FIFE {
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_RESMGR);

	AnimationManager::~AnimationManager() {

	}

	size_t AnimationManager::getMemoryUsed() const {
		size_t totalSize = 0;

		AnimationHandleMapConstIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		for ( ; it != itend; ++it) {
			totalSize += it->second->getSize();
		}

		return totalSize;
	}

	size_t AnimationManager::getTotalResourcesCreated() const {
		AnimationHandleMapConstIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_NOT_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t AnimationManager::getTotalResourcesLoaded() const {
		AnimationHandleMapConstIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t AnimationManager::getTotalResources() const {
		return m_animHandleMap.size();
	}

	AnimationPtr AnimationManager::create(IResourceLoader* loader){
		Animation* ptr = new Animation(loader);
		return add(ptr);
	}

	AnimationPtr AnimationManager::create(const std::string& name, IResourceLoader* loader){
		if (exists(name)) {
			FL_WARN(_log, LMsg("AnimationManager::create(std::string, IResourceLoader* loader) - ") << "Resource name " << name << " was previously created.  Returning original Animation...");
			return getPtr(name);
		}

		Animation* ptr = new Animation(name, loader);
		return add(ptr);
	}

	AnimationPtr AnimationManager::load(const std::string& name, IResourceLoader* loader) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);

		if (nit != m_animNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_NOT_LOADED ) {
				nit->second->load();
			}

			return nit->second;
		}

		//was not found so create and load resource
		AnimationPtr ptr = create(name, loader);
		ptr->load();

		if (ptr->getState() == IResource::RES_NOT_LOADED){
			FL_WARN(_log, LMsg("AnimationManager::load(std::string) - ") << "Resource name " << name << " was not found and could not be loaded.");
			remove(name);
		}

		return ptr;
	}

	AnimationPtr AnimationManager::add(Animation* res) {
		assert(res);
		assert(!(exists(res->getHandle()) || exists(res->getName())));

		AnimationPtr resptr(res);

		std::pair<AnimationHandleMapIterator, bool> returnValue;
		returnValue = m_animHandleMap.insert ( AnimationHandleMapPair(res->getHandle(), resptr));

		if (returnValue.second) {
			m_animNameMap.insert ( AnimationNameMapPair(returnValue.first->second->getName(), returnValue.first->second) );
		}
		else {
			FL_WARN(_log, LMsg("AnimationManager::add(IResource*) - ") << "Resource " << res->getName() << " already exists.... ignoring.");
		}

		return returnValue.first->second;
	}

	bool AnimationManager::exists(const std::string& name) {
		AnimationNameMapIterator it = m_animNameMap.find(name);
		if (it != m_animNameMap.end()) {
			return true;
		}

		return false;
	}

	bool AnimationManager::exists(ResourceHandle handle) {
		AnimationHandleMapConstIterator it = m_animHandleMap.find(handle);
		if (it != m_animHandleMap.end()) {
			return true;
		}

		return false;
	}

	void AnimationManager::reload(const std::string& name) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);

		if (nit != m_animNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			nit->second->load();
			return;
		}

		FL_WARN(_log, LMsg("AnimationManager::reload(std::string) - ") << "Resource name " << name << " not found.");
	}

	void AnimationManager::reload(ResourceHandle handle) {
		AnimationHandleMapIterator it = m_animHandleMap.find(handle);

		if ( it != m_animHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
			return;
		}

		FL_WARN(_log, LMsg("AnimationManager::reload(ResourceHandle) - ") << "Resource handle " << handle << " not found.");

	}

	void AnimationManager::reloadAll() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
		}
	}

	void AnimationManager::loadUnreferenced() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED){
				it->second->load();
				count++;
			}
		}
		FL_DBG(_log, LMsg("AnimationManager::loadUnreferenced() - ") << "Loaded " << count << " unreferenced resources.");
	}

	void AnimationManager::free(const std::string& name) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);

		if (nit != m_animNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("AnimationManager::free(std::string) - ") << "Resource name " << name << " not found.");
	}

	void AnimationManager::free(ResourceHandle handle) {
		AnimationHandleMapConstIterator it = m_animHandleMap.find(handle);
		if (it != m_animHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("AnimationManager::free(ResourceHandle) - ") << "Resource handle " << handle << " not found.");
	}

	void AnimationManager::freeAll() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		int32_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("AnimationManager::freeAll() - ") << "Freed all " << count << " resources.");
	}

	void AnimationManager::freeUnreferenced() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED ){
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("AnimationManager::freeUnreferenced() - ") << "Freed " << count << " unreferenced resources.");
	}

	void AnimationManager::remove(AnimationPtr& resource) {
		AnimationHandleMapIterator it = m_animHandleMap.find(resource->getHandle());
		AnimationNameMapIterator nit = m_animNameMap.find(resource->getName());

		if (it != m_animHandleMap.end()) {
			m_animHandleMap.erase(it);

			if (nit != m_animNameMap.end()) {
				m_animNameMap.erase(nit);
				return;
			}
			assert(false); //should never get here
		}

		FL_WARN(_log, LMsg("AnimationManager::remove(ResourcePtr&) - ") << "Resource " << resource->getName() << " was not found.");
	}

	void AnimationManager::remove(const std::string& name) {
		std::size_t handle;

		AnimationNameMapIterator nit = m_animNameMap.find(name);
		if (nit != m_animNameMap.end()) {
			handle = nit->second->getHandle();
			m_animNameMap.erase(nit);
		}
		else {
			FL_WARN(_log, LMsg("AnimationManager::remove(std::string) - ") << "Resource " << name << " was not found.");
			return;
		}

		AnimationHandleMapIterator it = m_animHandleMap.find(handle);
		if ( it != m_animHandleMap.end()) {
			m_animHandleMap.erase(it);
			return;
		}

		assert(false);  //should never get here
	}

	void AnimationManager::remove(ResourceHandle handle) {
		std::string name;

		AnimationHandleMapIterator it = m_animHandleMap.find(handle);

		if (it != m_animHandleMap.end()) {
			name = it->second->getName();
			m_animHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("AnimationManager::remove(ResourceHandle) - ") << "Resource handle " << handle << " was not found.");
			return;
		}

		AnimationNameMapIterator nit = m_animNameMap.find(name);
		if ( nit != m_animNameMap.end() ) {
			m_animNameMap.erase(nit);
			return;
		}

		assert(false);  //should never get here
	}

	void AnimationManager::removeAll() {
		//should always be equal
		assert (m_animHandleMap.size() == m_animNameMap.size());

		size_t count = m_animHandleMap.size();

		m_animHandleMap.clear();
		m_animNameMap.clear();

		FL_DBG(_log, LMsg("AnimationManager::removeAll() - ") << "Removed all " << count << " resources.");
	}

	void AnimationManager::removeUnreferenced() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		std::vector<int> imgHandles;

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if ( it->second.useCount() == 2) {
				imgHandles.push_back(it->second->getHandle());
				count++;
			}
		}

		for (std::vector<int>::iterator it = imgHandles.begin(); it != imgHandles.end(); ++it) {
			remove(*it);
		}

		FL_DBG(_log, LMsg("AnimationManager::removeUnreferenced() - ") << "Removed " << count << " unreferenced resources.");
	}

	AnimationPtr AnimationManager::get(const std::string& name) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);

		if (nit != m_animNameMap.end()) {
			if (nit->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				nit->second->load();
			}
			return nit->second;
		}

		//not found so attempt to create and load the resource
		AnimationPtr ptr = load(name);
		return ptr;
	}

	AnimationPtr AnimationManager::get(ResourceHandle handle) {
		AnimationHandleMapConstIterator it = m_animHandleMap.find(handle);
		if (it != m_animHandleMap.end()) {
			if (it->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				it->second->load();
			}
			return it->second;
		}

		FL_WARN(_log, LMsg("AnimationManager::get(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return AnimationPtr();
	}

	AnimationPtr AnimationManager::getPtr(const std::string& name) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);

		if (nit != m_animNameMap.end()) {
			return nit->second;
		}

		FL_WARN(_log, LMsg("AnimationManager::getPtr(std::string) - ") << "Resource " << name << " is undefined.");

		return AnimationPtr();
	}

	AnimationPtr AnimationManager::getPtr(ResourceHandle handle) {
		AnimationHandleMapConstIterator it = m_animHandleMap.find(handle);
		if (it != m_animHandleMap.end()) {
			return it->second;
		}

		FL_WARN(_log, LMsg("AnimationManager::getPtr(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return AnimationPtr();
	}

	ResourceHandle AnimationManager::getResourceHandle(const std::string& name) {
		AnimationNameMapIterator nit = m_animNameMap.find(name);
		if (nit != m_animNameMap.end()) {
			return nit->second->getHandle();
		}

		FL_WARN(_log, LMsg("AnimationManager::getResourceHandle(std::string) - ") << "Resource " << name << " is undefined.");

		return 0;
	}

	void AnimationManager::invalidate(const std::string& name) {
		AnimationNameMapIterator it = m_animNameMap.find(name);
		if (it != m_animNameMap.end()) {
			if (it->second->getState() == IResource::RES_LOADED){
				it->second.get()->invalidate();
			}
		}
	}

	void AnimationManager::invalidate(ResourceHandle handle) {
		AnimationHandleMapIterator it = m_animHandleMap.find(handle);
		if (it != m_animHandleMap.end()) {
			if (it->second->getState() == IResource::RES_LOADED) {
				it->second.get()->invalidate();
			}
		}
	}

	void AnimationManager::invalidateAll() {
		AnimationHandleMapIterator it = m_animHandleMap.begin(),
			itend = m_animHandleMap.end();

		for ( ; it != itend; ++it) {
			if (it->second->getState() == IResource::RES_LOADED) {
				it->second.get()->invalidate();
			}
		}

	}

} //FIFE
