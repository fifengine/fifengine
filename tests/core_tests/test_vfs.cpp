/***************************************************************************
 *   Copyright (C) 2005-2019 by the FIFE team                              *
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
