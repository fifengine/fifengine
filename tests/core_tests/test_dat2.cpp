// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes^
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
#include "util/time/timemanager.h"
#include "vfs/dat/dat2.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"

using FIFE::DAT2;
using FIFE::Exception;
using FIFE::RawData;
using FIFE::TimeManager;
using FIFE::VFS;
using FIFE::VFSDirectory;

static char const * const COMPRESSED_FILE = "tests/data/dat2vfstest.dat";
static char const * const RAW_FILE        = "tests/data/test.map";

TEST_CASE("DAT2::open decompresses LZSS entry correctly", "[dat2][vfs]")
{

    std::shared_ptr<TimeManager> timemanager = std::make_shared<TimeManager>();

    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->addSource(new VFSDirectory(vfs.get()));
    CHECK(vfs->exists(COMPRESSED_FILE));

    try {
        std::cout << "adding Source" << '\n';
        vfs->addSource(new DAT2(vfs.get(), COMPRESSED_FILE));
        std::cout << "added Source" << '\n';
    } catch (Exception& e) {
        std::cout << e.what() << '\n';
    }

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("dat2vfstest.map"));

    auto fraw  = std::unique_ptr<FIFE::RawData>(vfs->open(RAW_FILE));
    auto fcomp = std::unique_ptr<FIFE::RawData>(vfs->open("dat2vfstest.map"));

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
