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

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "resource.h"
#include "resource_manager.h"

namespace FIFE {

	IResrouceManager::~IResourceManager() {

	}

	void IResrouceManager::addChangeListener(IResourceManagerListener* listener);
	void IResrouceManager::removeChangeListener(IResourceManagerListener* listener);

	std::size_t IResrouceManager::getMemoryUsed() const {
		return 0;
	}

	ResroucePtr IResrouceManager::add(IResource* res){
		return ResourcePtr<IResource>;
	}

	void IResrouceManager::reload(const std::string& name) {

	}

	void IResrouceManager::reload(ResourcePtr& res) {

	}

	void IResrouceManager::reloadAll() {

	}

	void IResrouceManager::loadUnreferenced() {

	}

	void IResrouceManager::free(const std::string& name) {

	}

	void IResrouceManager::free(ResourceHandle handle) {

	}

	void IResrouceManager::freeAll() {

	}

	void IResrouceManager::freeUnreferenced() {

	}

	void IResrouceManager::remove(ResourcePtr& resource) {

	}

	void IResrouceManager::remove(const std::string& name) {

	}

	void IResrouceManager::remove(ResourceHandle handle) {

	}

	void IResrouceManager::removeAll() {

	}

	void IResrouceManager::removeUnreferenced() {

	}

	ResourcePtr IResrouceManager::get(const std::string& name) {

	}

	ResourcePtr IResrouceManager::get(ResourceHandle handle) {

	}

	ResrouceHandle IResrouceManager::getResourceHandle(const std::string& name) {

	}
} //FIFE
