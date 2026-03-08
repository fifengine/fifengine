// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "vfs/dat/dat1.h"
#include "vfs/dat/dat2.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"

using FIFE::DAT1;
using FIFE::RawData;
using FIFE::VFS;
using FIFE::VFSDirectory;

static const std::string COMPRESSED_FILE = "tests/data/dat1vfstest.dat";
static const std::string RAW_FILE        = "tests/data/test.map";

TEST_CASE("DAT1_test")
{

    std::shared_ptr<FIFE::VFS> vfs = std::make_shared<FIFE::VFS>();
    vfs->addSource(new FIFE::VFSDirectory(vfs.get()));
    CHECK(vfs->exists(COMPRESSED_FILE));

    vfs->addSource(new FIFE::DAT1(vfs.get(), COMPRESSED_FILE));

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("dat1vfstest.map"));

    FIFE::RawData* fraw  = vfs->open(RAW_FILE);
    FIFE::RawData* fcomp = vfs->open("dat1vfstest.map");

    CHECK((fraw->getDataLength()) == (fcomp->getDataLength()));
    // std::cout << "data length match, length = " << fcomp->getDataLength() << std::endl;

    unsigned int smaller_len = fraw->getDataLength();
    if (fcomp->getDataLength() < smaller_len) {
        smaller_len = fcomp->getDataLength();
    }

    uint8_t* d_raw  = new uint8_t[fraw->getDataLength()];
    uint8_t* d_comp = new uint8_t[fcomp->getDataLength()];
    fraw->readInto(d_raw, fraw->getDataLength());
    fcomp->readInto(d_comp, fcomp->getDataLength());
    // std::cout << "scanning data..." << std::endl;
    for (unsigned int i = 0; i < smaller_len; i++) {
        uint8_t rawc  = d_raw[i];
        uint8_t compc = d_comp[i];
        CHECK((compc) == (rawc));
        // std::cout
        //	<< "raw: " << std::setbase(16) << rawc
        //	<< " comp: " << std::setbase(16) << compc << std::endl;
        break;
    }
    // std::cout << "scanning finished" << std::endl;

    delete[] d_raw;
    delete[] d_comp;
    delete fraw;
    delete fcomp;
}