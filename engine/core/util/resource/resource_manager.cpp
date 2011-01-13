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
#include "resource_manager.h"
#include "resource.h"

namespace FIFE {

	IResourceManager::~IResourceManager() {

	}

	void addChangeListener(IResourceManagerListener* listener) {

	}

	void removeChangeListener(IResourceManagerListener* listener){

	}

	std::size_t IResourceManager::getMemoryUsed() const {
		return 0;
	}

	ResourcePtr IResourceManager::add(IResource* res){
		return ResourcePtr();
	}

	void IResourceManager::reload(const std::string& name) {

	}

	void IResourceManager::reload(ResourcePtr& res) {

	}

	void IResourceManager::reloadAll() {

	}

	void IResourceManager::loadUnreferenced() {

	}

	void IResourceManager::free(const std::string& name) {

	}

	void IResourceManager::free(ResourceHandle handle) {

	}

	void IResourceManager::freeAll() {

	}

	void IResourceManager::freeUnreferenced() {

	}

	void IResourceManager::remove(ResourcePtr& resource) {

	}

	void IResourceManager::remove(const std::string& name) {

	}

	void IResourceManager::remove(ResourceHandle handle) {

	}

	void IResourceManager::removeAll() {

	}

	void IResourceManager::removeUnreferenced() {

	}

	ResourcePtr IResourceManager::get(const std::string& name) {
		return ResourcePtr();
	}

	ResourcePtr IResourceManager::get(ResourceHandle handle) {
		return ResourcePtr();
	}

	ResourceHandle IResourceManager::getResourceHandle(const std::string& name) {
		return 0;
	}
} //FIFE
