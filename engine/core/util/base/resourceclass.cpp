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

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/resource/resource_location.h"

#include "exception.h"
#include "resourceclass.h"

namespace FIFE {
	ResourceClass::ResourceClass():
		FifeClass(), 
		m_location(NULL),
		m_poolid(-1) {
	}
	
	ResourceClass::~ResourceClass() {
		delete m_location;
	}
	
	const ResourceLocation& ResourceClass::getResourceLocation() {
		if (m_location) {
			return *m_location;
		}
		throw NotSet("Resource Location has not been set");
	}
	
	const std::string& ResourceClass::getResourceFile() {
		return getResourceLocation().getFilename();
	}
	
	void ResourceClass::setResourceLocation(const ResourceLocation& location) {
		delete m_location;
		m_location = location.clone();
	}
	
	void ResourceClass::setResourceFile(const std::string& filename) {
		setResourceLocation(ResourceLocation(filename));
	}
}; //FIFE

