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
#include <tinyxml.h>

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
	/** Logger to use for this source file.
	 *  @relates Logger
	 */
	static Logger _log(LM_RESMGR);

	ImageManager::~ImageManager() {

	}

	size_t ImageManager::getMemoryUsed() const {
		size_t totalSize = 0;

		ImageHandleMapConstIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		for ( ; it != itend; ++it) {
			totalSize += it->second->getSize();
		}

		return totalSize;
	}

	size_t ImageManager::getTotalResourcesCreated() const {
		ImageHandleMapConstIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_NOT_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t ImageManager::getTotalResourcesLoaded() const {
		ImageHandleMapConstIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();
		size_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED ) {
				count++;
			}
		}

		return count;
	}

	size_t ImageManager::getTotalResources() const {
		return m_imgHandleMap.size();
	}

	ImagePtr ImageManager::create(IResourceLoader* loader){
		Image* ptr = RenderBackend::instance()->createImage(loader);
		return add(ptr);
	}

	ImagePtr ImageManager::create(const std::string& name, IResourceLoader* loader){
		if (exists(name)) {
			FL_WARN(_log, LMsg("ImageManager::create(std::string, IResourceLoader* loader) - ") << "Resource name " << name << " was previously created.  Returning original Image...");
			return getPtr(name);
		}

		Image* ptr = RenderBackend::instance()->createImage(name, loader);
		return add(ptr);
	}

	ImagePtr ImageManager::load(const std::string& name, IResourceLoader* loader) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);

		if (nit != m_imgNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_NOT_LOADED ) {
				nit->second->load();
			}

			return nit->second;
		}

		//was not found so create and load resource
		ImagePtr ptr = create(name, loader);
		ptr->load();

		if (ptr->getState() == IResource::RES_NOT_LOADED){
			FL_WARN(_log, LMsg("ImageManager::load(std::string) - ") << "Resource name " << name << " was not found and could not be loaded.");
			remove(name);
		}

		return ptr;
	}

	ImagePtr ImageManager::loadBlank(uint32_t width, uint32_t height) {
		uint8_t* pixdata = new uint8_t[width * height * 4];
		memset(pixdata, 0, width * height * 4);
		Image* ptr = RenderBackend::instance()->createImage(pixdata, width, height);
		delete [] pixdata;
		ptr->setState(IResource::RES_LOADED);
		return add(ptr);
	}

	ImagePtr ImageManager::loadBlank(const std::string& name, uint32_t width, uint32_t height) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);
		if (nit != m_imgNameMap.end()) {
			remove(nit->second);
		}
		uint8_t* pixdata = new uint8_t[width * height * 4];
		memset(pixdata, 0, width * height * 4);
		Image* ptr = RenderBackend::instance()->createImage(name, pixdata, width, height);
		delete [] pixdata;
		ptr->setState(IResource::RES_LOADED);
		return add(ptr);
	}

	ImagePtr ImageManager::add(Image* res) {
		assert(res);
		assert(!(exists(res->getHandle()) || exists(res->getName())));

		ImagePtr resptr(res);

		std::pair<ImageHandleMapIterator, bool> returnValue;
		returnValue = m_imgHandleMap.insert ( ImageHandleMapPair(res->getHandle(), resptr));

		if (returnValue.second) {
			m_imgNameMap.insert ( ImageNameMapPair(returnValue.first->second->getName(), returnValue.first->second) );
		}
		else {
			FL_WARN(_log, LMsg("ImageManager::add(IResource*) - ") << "Resource " << res->getName() << " already exists.... ignoring.");
		}

		return returnValue.first->second;
	}

	bool ImageManager::exists(const std::string& name) {
		ImageNameMapIterator it = m_imgNameMap.find(name);
		if (it != m_imgNameMap.end()) {
			return true;
		}

		return false;
	}

	bool ImageManager::exists(ResourceHandle handle) {
		ImageHandleMapConstIterator it = m_imgHandleMap.find(handle);
		if (it != m_imgHandleMap.end()) {
			return true;
		}

		return false;
	}

	void ImageManager::reload(const std::string& name) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);

		if (nit != m_imgNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			nit->second->load();
			return;
		}

		FL_WARN(_log, LMsg("ImageManager::reload(std::string) - ") << "Resource name " << name << " not found.");
	}

	void ImageManager::reload(ResourceHandle handle) {
		ImageHandleMapIterator it = m_imgHandleMap.find(handle);

		if ( it != m_imgHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
			return;
		}

		FL_WARN(_log, LMsg("ImageManager::reload(ResourceHandle) - ") << "Resource handle " << handle << " not found.");

	}

	void ImageManager::reloadAll() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			it->second->load();
		}
	}

	void ImageManager::loadUnreferenced() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() != IResource::RES_LOADED){
				it->second->load();
				count++;
			}
		}
		FL_DBG(_log, LMsg("ImageManager::loadUnreferenced() - ") << "Loaded " << count << " unreferenced resources.");
	}

	void ImageManager::free(const std::string& name) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);

		if (nit != m_imgNameMap.end()) {
			if ( nit->second->getState() == IResource::RES_LOADED) {
				nit->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("ImageManager::free(std::string) - ") << "Resource name " << name << " not found.");
	}

	void ImageManager::free(ResourceHandle handle) {
		ImageHandleMapConstIterator it = m_imgHandleMap.find(handle);
		if (it != m_imgHandleMap.end()) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
			}
			return;
		}

		FL_WARN(_log, LMsg("ImageManager::free(ResourceHandle) - ") << "Resource handle " << handle << " not found.");
	}

	void ImageManager::freeAll() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		int32_t count = 0;

		for ( ; it != itend; ++it) {
			if ( it->second->getState() == IResource::RES_LOADED) {
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("ImageManager::freeAll() - ") << "Freed all " << count << " resources.");
	}

	void ImageManager::freeUnreferenced() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		int32_t count = 0;
		for ( ; it != itend; ++it) {
			if (it->second.useCount() == 2 && it->second->getState() == IResource::RES_LOADED ){
				it->second->free();
				count++;
			}
		}

		FL_DBG(_log, LMsg("ImageManager::freeUnreferenced() - ") << "Freed " << count << " unreferenced resources.");
	}

	void ImageManager::remove(ImagePtr& resource) {
		ImageHandleMapIterator it = m_imgHandleMap.find(resource->getHandle());
		ImageNameMapIterator nit = m_imgNameMap.find(resource->getName());

		if (it != m_imgHandleMap.end()) {
			m_imgHandleMap.erase(it);

			if (nit != m_imgNameMap.end()) {
				m_imgNameMap.erase(nit);
				return;
			}
			assert(false); //should never get here
		}

		FL_WARN(_log, LMsg("ImageManager::remove(ResourcePtr&) - ") << "Resource " << resource->getName() << " was not found.");
	}

	void ImageManager::remove(const std::string& name) {
		std::size_t handle;

		ImageNameMapIterator nit = m_imgNameMap.find(name);
		if (nit != m_imgNameMap.end()) {
			handle = nit->second->getHandle();
			m_imgNameMap.erase(nit);
		}
		else {
			FL_WARN(_log, LMsg("ImageManager::remove(std::string) - ") << "Resource " << name << " was not found.");
			return;
		}

		ImageHandleMapIterator it = m_imgHandleMap.find(handle);
		if ( it != m_imgHandleMap.end()) {
			m_imgHandleMap.erase(it);
			return;
		}

		assert(false);  //should never get here
	}

	void ImageManager::remove(ResourceHandle handle) {
		std::string name;

		ImageHandleMapIterator it = m_imgHandleMap.find(handle);

		if (it != m_imgHandleMap.end()) {
			name = it->second->getName();
			m_imgHandleMap.erase(it);
		}
		else {
			FL_WARN(_log, LMsg("ImageManager::remove(ResourceHandle) - ") << "Resource handle " << handle << " was not found.");
			return;
		}

		ImageNameMapIterator nit = m_imgNameMap.find(name);
		if ( nit != m_imgNameMap.end() ) {
			m_imgNameMap.erase(nit);
			return;
		}

		assert(false);  //should never get here
	}

	void ImageManager::removeAll() {
		//should always be equal
		assert (m_imgHandleMap.size() == m_imgNameMap.size());

		size_t count = m_imgHandleMap.size();

		m_imgHandleMap.clear();
		m_imgNameMap.clear();

		FL_DBG(_log, LMsg("ImageManager::removeAll() - ") << "Removed all " << count << " resources.");
	}

	void ImageManager::removeUnreferenced() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

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

		FL_DBG(_log, LMsg("ImageManager::removeUnreferenced() - ") << "Removed " << count << " unreferenced resources.");
	}

	ImagePtr ImageManager::get(const std::string& name) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);

		if (nit != m_imgNameMap.end()) {
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
		ImageHandleMapConstIterator it = m_imgHandleMap.find(handle);
		if (it != m_imgHandleMap.end()) {
			if (it->second->getState() != IResource::RES_LOADED){
				//resource is not loaded so load it
				it->second->load();
			}
			return it->second;
		}

		FL_WARN(_log, LMsg("ImageManager::get(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return ImagePtr();
	}

	ImagePtr ImageManager::getPtr(const std::string& name) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);

		if (nit != m_imgNameMap.end()) {
			return nit->second;
		}

		FL_WARN(_log, LMsg("ImageManager::getPtr(std::string) - ") << "Resource " << name << " is undefined.");

		return ImagePtr();
	}

	ImagePtr ImageManager::getPtr(ResourceHandle handle) {
		ImageHandleMapConstIterator it = m_imgHandleMap.find(handle);
		if (it != m_imgHandleMap.end()) {
			return it->second;
		}

		FL_WARN(_log, LMsg("ImageManager::getPtr(ResourceHandle) - ") << "Resource handle " << handle << " is undefined.");

		return ImagePtr();
	}

	ResourceHandle ImageManager::getResourceHandle(const std::string& name) {
		ImageNameMapIterator nit = m_imgNameMap.find(name);
		if (nit != m_imgNameMap.end()) {
			return nit->second->getHandle();
		}

		FL_WARN(_log, LMsg("ImageManager::getResourceHandle(std::string) - ") << "Resource " << name << " is undefined.");

		return 0;
	}

	void ImageManager::invalidate(const std::string& name) {
		ImageNameMapIterator it = m_imgNameMap.find(name);
		if (it != m_imgNameMap.end()) {
			if (it->second->getState() == IResource::RES_LOADED){
				it->second.get()->invalidate();
			}
		}
	}

	void ImageManager::invalidate(ResourceHandle handle) {
		ImageHandleMapIterator it = m_imgHandleMap.find(handle);
		if (it != m_imgHandleMap.end()) {
			if (it->second->getState() == IResource::RES_LOADED) {
				it->second.get()->invalidate();
			}
		}
	}

	void ImageManager::invalidateAll() {
		ImageHandleMapIterator it = m_imgHandleMap.begin(),
			itend = m_imgHandleMap.end();

		for ( ; it != itend; ++it) {
			if (it->second->getState() == IResource::RES_LOADED) {
				it->second.get()->invalidate();
			}
		}

	}

} //FIFE
