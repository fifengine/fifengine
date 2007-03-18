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
#include "vfs/vfs.h"
#include "vfs/vfshostsystem.h"
#include "map/loaders/fallout/dat1.h"
#include "vfs/raw/rawdata.h"
#include "log.h"
#include "exception.h"

using boost::unit_test::test_suite;
using namespace FIFE::map::loaders::fallout;
using namespace FIFE;

static const std::string COMPRESSED_FILE = "data/dat1test.dat";
static const std::string RAW_FILE = "data/dat1test.txt";

void test_decoder() {
	Log::setLogLevel(Log::LEVEL_MAX);
	boost::shared_ptr<VFS> vfs(new VFS());
	vfs->addSource(new VFSHostSystem());
	vfs->addSource(new DAT1(COMPRESSED_FILE));

	if ((!vfs->exists(COMPRESSED_FILE)) || (!vfs->exists("test.txt"))) {
		BOOST_ERROR("Test files not found");
	}

	RawDataPtr fraw = vfs->open(RAW_FILE);
	RawDataPtr fcomp = vfs->open("test.txt");

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
	std::cout << "scanning data..." << std::endl;
	try {
		for (unsigned int i = 0; i < fraw->getDataLength(); i++) {
			fraw->setIndex(i);
			fcomp->setIndex(i);
			uint8_t rawc = fraw->read8();
			uint8_t compc = fcomp->read8();
			if (rawc != compc) {
				BOOST_ERROR("Data mismatch");
				std::cout << "raw: " << std::setbase(16) << rawc << " comp: " << std::setbase(16) << compc << std::endl;
				break;
			}
		}
	} catch (const Exception& e) {
		BOOST_ERROR(e.getMessage());
	}
	std::cout << "scanning finished" << std::endl;
}

test_suite* init_unit_test_suite(int argc, char** argv) {
	test_suite* test = BOOST_TEST_SUITE("DAT1 tests");
	test->add(BOOST_TEST_CASE(&test_decoder), 0);
	return test;
}
