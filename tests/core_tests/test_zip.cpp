// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

// Platform specific includes
#include <catch2/catch_test_macros.hpp>

// FIFE includes
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
    std::shared_ptr<VFS> const vfs = std::make_shared<VFS>();
    vfs->addSource(std::make_unique<VFSDirectory>(vfs.get()));

    CHECK(vfs->exists(COMPRESSED_FILE));
    vfs->addSource(std::make_unique<ZipSource>(vfs.get(), COMPRESSED_FILE));

    CHECK_THROWS_AS(vfs->open("does-not-exist"), NotFound);
    std::set<std::string> const dirlist = vfs->listDirectories("ziptest_content");

    CHECK((dirlist.size()) == (4));
    CHECK(dirlist.contains("maps"));
    CHECK(dirlist.contains("testdir1"));
    CHECK(dirlist.contains("testdir2"));
    CHECK(dirlist.contains("testdir3"));

    std::set<std::string> filelist = vfs->listFiles("ziptest_content");
    CHECK((filelist.size()) == (0));
    filelist = vfs->listFiles("ziptest_content/testdir1");

    CHECK((filelist.size()) == (4));
    CHECK(filelist.contains("file"));
    CHECK(filelist.contains("file-a"));
    CHECK(filelist.contains("file-b"));
    CHECK(filelist.contains("file-c"));

    CHECK((vfs->listFiles("ziptest_content/testdir3").size()) == (0));
    CHECK((vfs->listDirectories("ziptest_content/testdir1").size()) == (0));

    CHECK(vfs->exists(RAW_FILE));
    CHECK(vfs->exists("ziptest_content/maps/test.map"));
    auto fraw  = std::unique_ptr<RawData>(vfs->open(RAW_FILE));
    auto fcomp = std::unique_ptr<RawData>(vfs->open("ziptest_content/maps/test.map"));

    CHECK((fraw->getDataLength()) == (fcomp->getDataLength()));
    auto smaller_len = std::min(fraw->getDataLength(), fcomp->getDataLength());

    std::vector<uint8_t> d_raw(fraw->getDataLength());
    std::vector<uint8_t> d_comp(fcomp->getDataLength());
    fraw->readInto(d_raw.data(), fraw->getDataLength());
    fcomp->readInto(d_comp.data(), fcomp->getDataLength());

    for (unsigned int i = 0; i < smaller_len; i++) {
        uint8_t const rawc  = d_raw.at(i);
        uint8_t const compc = d_comp.at(i);
        CHECK((rawc) == (compc));
    }
}
