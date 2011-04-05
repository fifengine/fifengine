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
#include "util/resource/resourcemanager.h"
#include "util/resource/resource.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "imagemanager.h"

namespace FIFE {
	static Logger _log(LM_RESMGR);

	ImageManager::~ImageManager() {

	}
	
	std::size_t ImageManager::getMemoryUsed() const {
		std::size_t totalSize = 0;
		
		ResourceHandleMapConstIterator it = m_resHandleMap.begin();
		
		for ( ; it != m_resHandleMap.end(); it++) {
			totalSize += it->second->getSize();
		}
		
		return totalSize;
	}
	
	uint32_t ImageManager::getTotalResourcesCreated() const {
		ResourceHandleMapConstIterator it = m_resHandleMap.begin();
		uint32_t count = 0;
		
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getState() == IResource::RES_NOT_LOADED ) {
				count++;
			}
		}
		
		return count;
	}
	
	uint32_t ImageManager::getTotalResourcesLoaded() const {
		ResourceHandleMapConstIterator it = m_resHandleMap.begin();
		uint32_t count = 0;
		
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getState() == IResource::RES_LOADED ) {
				count++;
			}
		}
		
		return count;
	}
	
	uint32_t ImageManager::getTotalResources() const {
		return m_resHandleMap.size();
	}

	
	ImagePtr ImageManager::create(const std::string& name, IResourceLoader* loader){
		Image* ptr = RenderBackend::instance()->createImage(name, loader);
		return add(ptr);
	}
	
	ImagePtr ImageManager::load(const std::string& name, IResourceLoader* loader) {
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		
		if (nit != m_resNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_NOT_LOADED ) {
				nit->second->load();
			}
			
			return nit->second;
		}
		
		//was not found so create and load resource
		ImagePtr ptr = create(name, loader);
		ptr->load();
		
		if (ptr->getState() == IResource::RES_NOT_LOADED){
			FL_WARN(_log, LMsg("IResourceManager::load(std::string) - ") << "Resource name " << name << " was not found and could not be loaded.");
			remove(name);
		}
		
		return ptr;
	}
	
	ImagePtr ImageManager::add(Image* res) {
		assert(res);
		
		ImagePtr resptr(res);
		
		std::pair<ResourceHandleMapIterator, bool> returnValue;
		returnValue = m_resHandleMap.insert ( ResourceHandleMapPair(res->getHandle(), resptr));
		
		if (returnValue.second) {
			m_resNameMap.insert ( ResourceNameMapPair(returnValue.first->second->getName(), returnValue.first->second) );
		}
		else {
			FL_WARN(_log, LMsg("IResourceManager::add(IResource*) - ") << "Resource " << res->getName() << " already exists.... ignoring.");
		}
		
		return returnValue.first->second;
	}
	
	bool ImageManager::exists(const std::string& name) {
		ResourceNameMapIterator it = m_resNameMap.find(name);
		if (it != m_resNameMap.end()) {
			return true;
		}
		
		return false;
	}
	
	bool ImageManager::exists(ResourceHandle handle) {
		ResourceHandleMapConstIterator it = m_resHandleMap.find(handle);
		if (it != m_resHandleMap.end()) {
			return true;
		}
		
		return false;
	}
	
	void ImageManager::reload(const std::string& name) {
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		
		if (nit != m_resNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			nit->second->load();
			return;
		}
		
		FL_WARN(_log, LMsg("IResourceManager::reload(std::string) - ") << "Resource name " << name << " not found.");
	}
	
	void ImageManager::reload(ResourceHandle handle) {
		ResourceHandleMapIterator it = m_resHandleMap.find(handle);
		
		if ( it != m_resHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
			return;
		}
		
		FL_WARN(_log, LMsg("IResourceManager::reload(ResourceHandle) - ") << "Resource handle " << handle << " not found.");
		
	}
	
	void ImageManager::reloadAll() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();
		
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
		}
	}
	
	void ImageManager::loadUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();
		
		int32_t count = 0;
		for ( ; it != m_resHandleMap.end(); it++) {
			if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED){
				it->second->load();
				count++;
			}
		}
		FL_DBG(_log, LMsg("IResourceManager::loadUnreferenced() - ") << "Loaded " << count << " unreferenced resources.");
	}
	
	void ImageManager::free(const std::string& name) {
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		
		if (nit != m_resNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			return;
		}
		
		FL_WARN(_log, LMsg("IResourceManager::free(std::string) - ") << "Resource name " << name << " not found.");
	}
	
	void ImageManager::free(ResourceHandle handle) {
		ResourceHandleMapConstIterator it = m_resHandleMap.find(handle);
		if (it != m_resHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			return;
		}
		
		FL_WARN(_log, LMsg("IResourceManager::free(ResourceHandle) - ") << "Resource handle " << handle << " not found.");
	}
	
	void ImageManager::freeAll() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();
		
		int32_t count = 0;
		
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
				count++;
			}
		}
		
		FL_DBG(_log, LMsg("IResourceManager::freeAll() - ") << "Freed all " << count << " resources.");
	}
	
	void ImageManager::freeUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();
		
		int32_t count = 0;
		for ( ; it != m_resHandleMap.end(); it++) {
			if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED ){
				it->second->free();
				count++;
			}
		}
		
		FL_DBG(_log, LMsg("IResourceManager::freeUnreferenced() - ") << "Freed " << count << " unreferenced resources.");
	}
	
	void ImageManager::remove(ImagePtr& resource) {
		ResourceHandleMapIterator it = m_resHandleMap.find(resource->getHandle());
		ResourceNameMapIterator nit = m_resNameMap.find(resource->getName());
		
		if (it != m_resHandleMap.end()) {
			m_resHandleMap.erase(it);
			
			if (nit != m_resNameMap.end()) {
				m_resNameMap.erase(nit);
				return;
			}
			assert(false); //should never get here
		}
		
		FL_WARN(_log, LMsg("IResourceManager::remove(ResourcePtr&) - ") << "Resource " << resource->getName() << " was not found.");
	}
	
	void ImageManager::remove(const std::string& name) {
		std::size_t handle;
		
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		if (nit != m_resNameMap.end()) {
			handle = nit->second->getHandle();
			m_resNameMap.erase(nit);
		}
		else {
			FL_WARN(_log, LMsg("IResourceManager::remove(std::string) - ") << "Resource " << name << " was not found.");
			return;
		}
		
		ResourceHandleMapIterator it = m_resHandleMap.find(handle);
		if ( it != m_resHandleMap.end()) {
			m_resHandleMap.erase(it);
			return;
		}
		
		assert(false);  //should never get here
	}
	
	void ImageManager::remove(ResourceHandle handle) {
		std::string name;
		
		ResourceHandleMapIterator it = m_resHandleMap.find(handle);
		
		if (it != m_resHandleMap.end()) {
			name = it->second->getName();
			m_resHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("IResourceManager::remove(ResourceHandle) - ") << "Resource handle " << handle << " was not found.");
			return;
		}
		
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		if ( nit != m_resNameMap.end() ) {
			m_resNameMap.erase(nit);
			return;
		}
		
		assert(false);  //should never get here
	}
	
	void ImageManager::removeAll() {
		int32_t count = m_resHandleMap.size();
		int32_t count2 = m_resNameMap.size();
		
		//should always be equal
		assert (count == count2);
		
		m_resHandleMap.clear();
		m_resNameMap.clear();
		
		FL_DBG(_log, LMsg("IResourceManager::removeAll() - ") << "Removed all " << count << " resources.");
	}
	
	void ImageManager::removeUnreferenced() {
		ResourceHandleMapIterator it = m_resHandleMap.begin();
		
		int32_t count = 0;
		for ( ; it != m_resHandleMap.end(); it++) {
			if ( it->second.useCount() == 2) {
				remove(it->second->getHandle());
				count++;
			}
		}
		
		FL_DBG(_log, LMsg("IResourceManager::removeUnreferenced() - ") << "Removed " << count << " unreferenced resources.");
	}
	
	ImagePtr ImageManager::get(const std::string& name) {
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		
		if (nit != m_resNameMap.end()) {
			if (nit->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				nit->second->load();
			}
			return nit->second;
		}
		
		//not found so attempt to create and load the resource
		ImagePtr ptr = load(name);
		return ptr;
	}
	
	ImagePtr ImageManager::get(ResourceHandle handle) {
		ResourceHandleMapConstIterator it = m_resHandleMap.find(handle);
		if (it != m_resHandleMap.end()) {
			if (it->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				it->second->load();
			}
			return it->second;
		}
		
		FL_WARN(_log, LMsg("IResourceManager::get(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");
		
		return ImagePtr();
	}
	
	ResourceHandle ImageManager::getResourceHandle(const std::string& name) {
		ResourceNameMapIterator nit = m_resNameMap.find(name);
		if (nit != m_resNameMap.end()) {
			return nit->second->getHandle();
		}
		
		FL_WARN(_log, LMsg("IResourceManager::getResourceHandle(std::string) - ") << "Resource " << name << " is undefined.");
		
		return 0;
	}
	

} //FIFE
