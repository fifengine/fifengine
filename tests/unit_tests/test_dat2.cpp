/***************************************************************************
*   Copyright (C) 2005-2007 by the FIFE Team                              *
*   fife-public@lists.sourceforge.net                                     *
*   This file is part of FIFE.                                            *
*                                                                         *
*   FIFE is free software; you can redistribute it and/or modify          *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
***************************************************************************/

// Standard C++ library includes

// 3rd party library includes
#include <boost/test/unit_test.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "map/map.h"
#include "map/elevation.h"
#include "map/layer.h"
#include "map/objectinfo.h"
#include "map/factory.h"
#include "vfs/vfssourcefactory.h"
#include "vfs/vfs.h"
#include "util/settingsmanager.h"
#include "util/time/timemanager.h"
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "map/loaders/fallout/dat2.h"
#include "vfs/raw/rawdata.h"
#include "util/log.h"
#include "util/exception.h"


using boost::unit_test::test_suite;
using namespace FIFE::map;
using namespace FIFE;

// Environment
struct environment {
	boost::shared_ptr<SettingsManager> settings;
	boost::shared_ptr<TimeManager> timemanager;
	boost::shared_ptr<VFSSourceFactory> vfssources;
	boost::shared_ptr<VFS> vfs;
	boost::shared_ptr<Factory> factory;

	environment()
		: settings(new SettingsManager()),
		  timemanager(new TimeManager()),
		  vfssources(new VFSSourceFactory()),
		  vfs(new VFS()),
		  factory(new Factory()) {}
};

using boost::unit_test::test_suite;
using namespace FIFE::map::loaders::fallout;
using namespace FIFE;

static const std::string COMPRESSED_FILE = "../data/dat2vfstest.dat";
static const std::string RAW_FILE = "../data/test.map";

void test_decoder() {
	environment env;

	Log::setLogLevel(Log::LEVEL_MAX);
	VFS* vfs = VFS::instance();
	vfs->addSource(new VFSHostSystem());

	if ((!vfs->exists(COMPRESSED_FILE))) {
		BOOST_ERROR("Test source " << COMPRESSED_FILE << " not found");
		return;
	}

	vfs->addSource(new DAT2(COMPRESSED_FILE));

	if ((!vfs->exists(RAW_FILE)) || (!vfs->exists("dat2vfstest.map"))) {
		BOOST_ERROR("Test files not found");
	}

	RawDataPtr fraw = vfs->open(RAW_FILE);
	RawDataPtr fcomp = vfs->open("dat2vfstest.map");

	if (fraw->getDataLength() != fcomp->getDataLength()) {
		std::cout << "raw length = " << fraw->getDataLength() \
				  << ", compressed length = " << fcomp->getDataLength() << std::endl;
		BOOST_ERROR("Data length mismatch");
	} else {
		std::cout << "data length match, length = " << fcomp->getDataLength() << std::endl;
	}

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
		if (rawc != compc) {
			BOOST_ERROR("Data mismatch");
			std::cout 
				<< "raw: " << std::setbase(16) << rawc 
				<< " comp: " << std::setbase(16) << compc << std::endl;
			break;
		}
	}
	std::cout << "scanning finished" << std::endl;
	delete[] d_raw;
	delete[] d_comp;
}

test_suite* init_unit_test_suite(int argc, char** const argv) {
	test_suite* test = BOOST_TEST_SUITE("DAT2 tests");
	test->add(BOOST_TEST_CASE(&test_decoder), 0);
	return test;
}
