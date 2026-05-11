// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <string>

#include "fife_unittest.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"
#include "vfs/zip/zipsource.h"

using FIFE::NotFound;
using FIFE::RawData;
using FIFE::VFS;
using FIFE::VFSDirectory;
using FIFE::ZipSource;

static char const * const COMPRESSED_FILE = "tests/data/testmap.zip";
static char const * const RAW_FILE        = "tests/data/test.map";

TEST_CASE("ZipSource::open decompresses stored and deflated entries correctly", "[core][zip]")
{
    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->addSource(new VFSDirectory(vfs.get()));

    CHECK(vfs->exists(COMPRESSED_FILE));
    vfs->addSource(new ZipSource(vfs.get(), COMPRESSED_FILE));

    CHECK_THROWS_AS(vfs->open("does-not-exist"), NotFound);
    std::set<std::string> dirlist = vfs->listDirectories("ziptest_content");

    CHECK_EQ(dirlist.size(), 4);
    CHECK(dirlist.contains("maps"));
    CHECK(dirlist.contains("testdir1"));
    CHECK(dirlist.contains("testdir2"));
    CHECK(dirlist.contains("testdir3"));

    std::set<std::string> filelist = vfs->listFiles("ziptest_content");
    CHECK_EQ(filelist.size(), 0);
    filelist = vfs->listFiles("ziptest_content/testdir1");

    CHECK_EQ(filelist.size(), 4);
    CHECK(filelist.contains("file"));
    CHECK(filelist.contains("file-a"));
    CHECK(filelist.contains("file-b"));
    CHECK(filelist.contains("file-c"));

    CHECK_EQ(vfs->listFiles("ziptest_content/testdir3").size(), 0);
    CHECK_EQ(vfs->listDirectories("ziptest_content/testdir1").size(), 0);

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("ziptest_content/maps/test.map"));
    auto fraw  = std::unique_ptr<RawData>(vfs->open(RAW_FILE));
    auto fcomp = std::unique_ptr<RawData>(vfs->open("ziptest_content/maps/test.map"));

    CHECK_EQ(fraw->getDataLength(), fcomp->getDataLength());
    std::cout << "9" << '\n';
    unsigned int smaller_len = fraw->getDataLength();
    if (fcomp->getDataLength() < smaller_len) {
        smaller_len = fcomp->getDataLength();
    }

    std::vector<uint8_t> d_raw(fraw->getDataLength());
    std::vector<uint8_t> d_comp(fcomp->getDataLength());
    fraw->readInto(d_raw.data(), fraw->getDataLength());
    fcomp->readInto(d_comp.data(), fcomp->getDataLength());

    std::cout << "scanning data..." << '\n';
    for (unsigned int i = 0; i < smaller_len; i++) {
        uint8_t rawc  = d_raw[i];
        uint8_t compc = d_comp[i];
        CHECK_EQ(rawc, compc);
    }
    std::cout << "scanning finished" << '\n';
}
