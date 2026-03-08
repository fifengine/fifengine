// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>

// Platform specific includes
#include "fife_unittest.h"

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/rect.h"
#include "vfs/directoryprovider.h"
#include "vfs/raw/rawdata.h"
#include "vfs/vfs.h"
#include "vfs/vfsdirectory.h"

static const std::string FIFE_TEST_DIR = "fifetestdir";

using FIFE::VFS;
using FIFE::VFSDirectory;

TEST_CASE("test_is_directory")
{
    const std::filesystem::path test_dir = std::filesystem::current_path() / FIFE_TEST_DIR;

    const std::filesystem::path nested_test_dir = test_dir / FIFE_TEST_DIR;

    const std::string test_dir_rel = test_dir.lexically_relative(std::filesystem::current_path()).string();

    const std::string nested_test_dir_rel =
        nested_test_dir.lexically_relative(std::filesystem::current_path()).string();

    std::error_code ec;
    std::filesystem::remove_all(test_dir, ec);

    std::shared_ptr<VFS> vfs = std::make_shared<VFS>();
    vfs->addSource(new VFSDirectory(vfs.get()));

    CHECK(vfs->isDirectory(""));
    CHECK(vfs->isDirectory("/"));

    CHECK(!vfs->isDirectory(test_dir_rel));
    std::filesystem::create_directory(test_dir);
    CHECK(vfs->isDirectory(test_dir_rel));
    CHECK(!vfs->isDirectory(nested_test_dir_rel));
    std::filesystem::create_directories(nested_test_dir);
    CHECK(vfs->isDirectory(nested_test_dir_rel));

    std::filesystem::remove_all(test_dir, ec);
}