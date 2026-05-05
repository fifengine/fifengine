// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <set>
#include <string>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
#include "util/base/exception.h"
#include "util/time/timemanager.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/zip/zipsource.h"

using FIFE::NotFound;
using FIFE::RawData;
using FIFE::TimeManager;
using FIFE::VFS;
using FIFE::VFSDirectory;
using FIFE::ZipSource;

// Environment
struct environment
{
        std::shared_ptr<TimeManager> timemanager;

        environment() : timemanager(std::make_shared<TimeManager>())
        {
        }
};

static std::string const COMPRESSED_FILE = "tests/data/testmap.zip";
using FIFE::NotFound;
using FIFE::RawData;
using FIFE::VFS;
using FIFE::VFSDirectory;
using FIFE::ZipSource;
static std::string const RAW_FILE = "tests/data/test.map";

TEST_CASE("test_decoder")
{
    environment env;
    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->addSource(new VFSDirectory(vfs.get()));

    CHECK(vfs->exists(COMPRESSED_FILE));
    vfs->addSource(new ZipSource(vfs.get(), COMPRESSED_FILE));

    CHECK_THROWS_AS(vfs->open("does-not-exist"), NotFound);
    std::set<std::string> dirlist = vfs->listDirectories("ziptest_content");

    CHECK_EQ(dirlist.size(), 4);
    CHECK(std::find(dirlist.begin(), dirlist.end(), "maps") != dirlist.end());
    CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir1") != dirlist.end());
    CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir2") != dirlist.end());
    CHECK(std::find(dirlist.begin(), dirlist.end(), "testdir3") != dirlist.end());

    std::set<std::string> filelist = vfs->listFiles("ziptest_content");
    CHECK_EQ(filelist.size(), 0);
    filelist = vfs->listFiles("ziptest_content/testdir1");

    CHECK_EQ(filelist.size(), 4);
    CHECK(std::find(filelist.begin(), filelist.end(), "file") != filelist.end());
    CHECK(std::find(filelist.begin(), filelist.end(), "file-a") != filelist.end());
    CHECK(std::find(filelist.begin(), filelist.end(), "file-b") != filelist.end());
    CHECK(std::find(filelist.begin(), filelist.end(), "file-c") != filelist.end());

    CHECK_EQ(vfs->listFiles("ziptest_content/testdir3").size(), 0);
    CHECK_EQ(vfs->listDirectories("ziptest_content/testdir1").size(), 0);

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("ziptest_content/maps/test.map"));
    RawData* fraw  = vfs->open(RAW_FILE);
    RawData* fcomp = vfs->open("ziptest_content/maps/test.map");

    CHECK_EQ(fraw->getDataLength(), fcomp->getDataLength());
    std::cout << "9" << '\n';
    unsigned int smaller_len = fraw->getDataLength();
    if (fcomp->getDataLength() < smaller_len) {
        smaller_len = fcomp->getDataLength();
    }

    uint8_t* d_raw  = new uint8_t[fraw->getDataLength()];
    uint8_t* d_comp = new uint8_t[fcomp->getDataLength()];
    fraw->readInto(d_raw, fraw->getDataLength());
    fcomp->readInto(d_comp, fcomp->getDataLength());

    std::cout << "scanning data..." << '\n';
    for (unsigned int i = 0; i < smaller_len; i++) {
        uint8_t rawc  = d_raw[i];
        uint8_t compc = d_comp[i];
        CHECK_EQ(rawc, compc);
    }
    std::cout << "scanning finished" << '\n';
    delete[] d_raw;
    delete[] d_comp;
    delete fraw;
    delete fcomp;
}
