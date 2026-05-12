// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "directoryprovider.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "filesystem.h"
#include "util/base/exception.h"
#include "vfs.h"
#include "vfsdirectory.h"

namespace FIFE
{
    bool DirectoryProvider::isReadable(std::string const & path) const
    {
        return fs::is_directory(fs::path(path));
    }

    FIFE::VFSSource* DirectoryProvider::createSource(std::string const & path)
    {
        if (isReadable(path)) {
            VFSSource* source = nullptr;
            if (hasSource(path)) {
                source = m_sources[path];
            } else {
                source          = new VFSDirectory(getVFS(), path);
                m_sources[path] = source;
            }
            return source;
        }
        throw Exception("Path " + path + " is not readable.");
    }

    VFSSource* DirectoryProvider::getSource(std::string const & path) const
    {
        VFSSource* source = nullptr;
        if (hasSource(path)) {
            source = m_sources.find(path)->second;
        }
        return source;
    }

    bool DirectoryProvider::hasSource(std::string const & path) const
    {
        return m_sources.contains(path);
    }
} // namespace FIFE
