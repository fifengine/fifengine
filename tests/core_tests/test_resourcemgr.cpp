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
#include <iostream>
#include <iomanip>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/sharedptr.h"
#include "util/base/fife_stdint.h"
#include "util/resource/resource_manager.h"
#include "util/resource/resource.h"

using namespace FIFE;

class TestResource : public IResource {
public:
	TestResource(const std::string& name, IResourceLoader* loader = 0) : IResource(name, loader) { }

	virtual size_t getSize() { return getName().size(); }

	virtual void load() { m_state = RES_LOADED; }
	virtual void free() { m_state = RES_NOT_LOADED; }
};

class TestManager : public IResourceManager {
public:
	TestManager() : IResourceManager() {}

	//creates a blank resource but does not load it immediately
	virtual ResourcePtr create(const std::string& name, IResourceLoader* loader = 0){
		TestResource* ptr = new TestResource(name, loader);
		return add(ptr);
	}

	//creates a resources and loads it
	virtual ResourcePtr load(const std::string& name, IResourceLoader* loader = 0){
		TestResource* ptr = new TestResource(name, loader);
		ptr->load();
		return add(ptr);
	}
};

TEST(case1) {
	TestManager resmgr;

	ResourcePtr ptr = resmgr.create("test_resource1");

	CHECK(ptr->getState() == IResource::RES_NOT_LOADED);

	ResourcePtr ptr2 = resmgr.get("test_resource1");

	CHECK(ptr2->getState() == IResource::RES_LOADED);

	CHECK(ptr == ptr2);

	resmgr.free(ptr2->getHandle());

	CHECK(ptr2->getState() == IResource::RES_NOT_LOADED);
}

int32_t main() {
	return UnitTest::RunAllTests();
}
