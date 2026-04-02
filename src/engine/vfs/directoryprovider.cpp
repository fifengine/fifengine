// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "directoryprovider.h"
#include "filesystem.h"
#include "vfs.h"
#include "vfsdirectory.h"

namespace FIFE
{
    bool DirectoryProvider::isReadable(const std::string& path) const
    {
        return fs::is_directory(fs::path(path));
    }

    FIFE::VFSSource* DirectoryProvider::createSource(const std::string& path)
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

    VFSSource* DirectoryProvider::getSource(const std::string& path) const
    {
        VFSSource* source = nullptr;
        if (hasSource(path)) {
            source = m_sources.find(path)->second;
        }
        return source;
    }

    bool DirectoryProvider::hasSource(const std::string& path) const
    {
        return m_sources.contains(path);
    }
} // namespace FIFE
