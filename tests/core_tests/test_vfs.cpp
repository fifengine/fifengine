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

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem/convenience.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "util/structures/rect.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"
#include "vfs/directoryprovider.h"

static const std::string FIFE_TEST_DIR = "fifetestdir";
using namespace FIFE;

TEST(test_is_directory)
{
	boost::shared_ptr<VFS> vfs(new VFS());
	vfs->addSource(new VFSDirectory(vfs.get()));

	if(boost::filesystem::exists(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR)) {
		boost::filesystem::remove(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR);
	}

	if(boost::filesystem::exists(FIFE_TEST_DIR)) {
		boost::filesystem::remove(FIFE_TEST_DIR);
	}
	CHECK(vfs->isDirectory(""));
	CHECK(vfs->isDirectory("/"));

	CHECK(!vfs->isDirectory(FIFE_TEST_DIR));
	boost::filesystem::create_directory(FIFE_TEST_DIR);
	CHECK(vfs->isDirectory(FIFE_TEST_DIR));
	CHECK(!vfs->isDirectory(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR));
	boost::filesystem::create_directories(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR);
	CHECK(vfs->isDirectory(FIFE_TEST_DIR+"/"+FIFE_TEST_DIR));

}

int main() {
	return UnitTest::RunAllTests();
}
