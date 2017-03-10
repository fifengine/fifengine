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
#include <iostream>
#include <iomanip>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs/vfs.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/zip/zipsource.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"

using namespace FIFE;

// Environment
struct environment {
	boost::shared_ptr<TimeManager> timemanager;

	environment()
		: timemanager(new TimeManager()) {}
};

using namespace FIFE;

static const std::string COMPRESSED_FILE = "tests/data/testmap.zip";
static const std::string RAW_FILE = "tests/data/test.map";

TEST(test_decoder) {
	environment env;
	boost::shared_ptr<VFS> vfs(new VFS());
	vfs->addSource(new VFSDirectory(vfs.get()));
	
	CHECK(vfs->exists(COMPRESSED_FILE));
	vfs->addSource(new ZipSource(vfs.get(), COMPRESSED_FILE));
	
	CHECK_THROW(vfs->open("does-not-exist"), NotFound);
	std::set<std::string> dirlist = vfs->listDirectories("ziptest_content");

	CHECK(dirlist.size() == 4);
	CHECK(std::find(dirlist.begin(), dirlist.end(), "maps") != dirlist.end());
	CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir1") != dirlist.end());
	CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir2") != dirlist.end());
	CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir3") != dirlist.end());

	std::set<std::string> filelist = vfs->listFiles("ziptest_content");
	CHECK(filelist.size() == 0);
	filelist = vfs->listFiles("ziptest_content/testdir1");

	CHECK(filelist.size() == 4);
	CHECK(std::find(filelist.begin(), filelist.end(), "file") != filelist.end());
	CHECK(std::find(filelist.begin(), filelist.end(), "file-a") != filelist.end());
	CHECK(std::find(filelist.begin(), filelist.end(), "file-b") != filelist.end());
	CHECK(std::find(filelist.begin(), filelist.end(), "file-c") != filelist.end());

	CHECK(vfs->listFiles("ziptest_content/testdir3").size() == 0);
	CHECK(vfs->listDirectories("ziptest_content/testdir1").size() == 0);

	CHECK(vfs->exists(RAW_FILE) && vfs->exists("ziptest_content/maps/test.map"));
	RawData* fraw = vfs->open(RAW_FILE);
	RawData* fcomp = vfs->open("ziptest_content/maps/test.map");

	CHECK(fraw->getDataLength() == fcomp->getDataLength());
	std::cout << "9" << std::endl;
	unsigned int smaller_len = fraw->getDataLength();
	if (fcomp->getDataLength() < smaller_len) {
		smaller_len = fcomp->getDataLength();
	}

	uint8_t* d_raw  = new uint8_t[fraw->getDataLength()];
	uint8_t* d_comp = new uint8_t[fcomp->getDataLength()];
	fraw->readInto(d_raw,fraw->getDataLength());
	fcomp->readInto(d_comp,fcomp->getDataLength());

	std::cout << "scanning data..." << std::endl;
	for (unsigned int i = 0; i < smaller_len; i++) {
		uint8_t rawc =  d_raw[i];
		uint8_t compc = d_comp[i];
		CHECK(rawc == compc);
		
	}
	std::cout << "scanning finished" << std::endl;
	delete[] d_raw;
	delete[] d_comp;
	delete fraw;
	delete fcomp;
}

int main() {
	return UnitTest::RunAllTests();
}
