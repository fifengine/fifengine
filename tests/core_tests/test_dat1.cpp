// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
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

static char const * const COMPRESSED_FILE = "tests/data/dat1vfstest.dat";
static char const * const RAW_FILE        = "tests/data/test.map";

TEST_CASE("DAT1::open decompresses stored entry correctly", "[dat1][vfs]")
{

    std::shared_ptr<FIFE::VFS> vfs = std::make_shared<FIFE::VFS>();
    vfs->addSource(new FIFE::VFSDirectory(vfs.get()));
    CHECK(vfs->exists(COMPRESSED_FILE));

    vfs->addSource(new FIFE::DAT1(vfs.get(), COMPRESSED_FILE));

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("dat1vfstest.map"));

    auto fraw  = std::unique_ptr<FIFE::RawData>(vfs->open(RAW_FILE));
    auto fcomp = std::unique_ptr<FIFE::RawData>(vfs->open("dat1vfstest.map"));

    CHECK((fraw->getDataLength()) == (fcomp->getDataLength()));
    // std::cout << "data length match, length = " << fcomp->getDataLength() << '\n';

    auto smaller_len = std::min(fraw->getDataLength(), fcomp->getDataLength());

    std::vector<uint8_t> d_raw(fraw->getDataLength());
    std::vector<uint8_t> d_comp(fcomp->getDataLength());
    fraw->readInto(d_raw.data(), fraw->getDataLength());
    fcomp->readInto(d_comp.data(), fcomp->getDataLength());
    // std::cout << "scanning data..." << '\n';
    for (unsigned int i = 0; i < smaller_len; i++) {
        uint8_t rawc  = d_raw[i];
        uint8_t compc = d_comp[i];
        CHECK((compc) == (rawc));
        // std::cout
        //	<< "raw: " << std::setbase(16) << rawc
        //	<< " comp: " << std::setbase(16) << compc << '\n';
        break;
    }
    // std::cout << "scanning finished" << '\n';
}
