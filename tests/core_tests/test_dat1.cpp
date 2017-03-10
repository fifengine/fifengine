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
#include "vfs/vfsdirectory.h"
#include "vfs/dat/dat1.h"
#include "vfs/dat/dat2.h"
#include "vfs/raw/rawdata.h"
#include "util/base/exception.h"

using namespace FIFE;

static const std::string COMPRESSED_FILE = "tests/data/dat1vfstest.dat";
static const std::string RAW_FILE = "tests/data/test.map";

TEST(DAT1_test){
	
	boost::shared_ptr<FIFE::VFS> vfs(new FIFE::VFS());
	vfs->addSource(new FIFE::VFSDirectory(vfs.get()));
	CHECK(vfs->exists(COMPRESSED_FILE));
	

	vfs->addSource(new FIFE::DAT1(vfs.get(), COMPRESSED_FILE));
	
	CHECK(vfs->exists(RAW_FILE));
	CHECK(vfs->exists("dat1vfstest.map"));
	
	FIFE::RawData* fraw = vfs->open(RAW_FILE);
	FIFE::RawData* fcomp = vfs->open("dat1vfstest.map");

	CHECK_EQUAL(fraw->getDataLength(), fcomp->getDataLength());
	//std::cout << "data length match, length = " << fcomp->getDataLength() << std::endl;

	unsigned int smaller_len = fraw->getDataLength();
	if (fcomp->getDataLength() < smaller_len) {
		smaller_len = fcomp->getDataLength();
	}

	uint8_t* d_raw  = new uint8_t[fraw->getDataLength()];
	uint8_t* d_comp = new uint8_t[fcomp->getDataLength()];
	fraw->readInto(d_raw,fraw->getDataLength());
	fcomp->readInto(d_comp,fcomp->getDataLength());
	//std::cout << "scanning data..." << std::endl;
	for (unsigned int i = 0; i < smaller_len; i++) {
		uint8_t rawc =  d_raw[i];
		uint8_t compc = d_comp[i];
		CHECK_EQUAL(compc, rawc);
		//std::cout 
		//	<< "raw: " << std::setbase(16) << rawc 
		//	<< " comp: " << std::setbase(16) << compc << std::endl;
		break;
		
	
	}
	//std::cout << "scanning finished" << std::endl;
	
	delete[] d_raw;
	delete[] d_comp;
	delete fraw;
	delete fcomp;
}

int main() {
	return UnitTest::RunAllTests();
}
