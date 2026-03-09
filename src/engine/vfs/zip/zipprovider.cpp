// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <cstddef>
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "vfs/vfs.h"

#include "zipprovider.h"
#include "zipsource.h"

namespace FIFE
{
    bool ZipProvider::isReadable(const std::string& file) const
    {
        // File name must have a .zip extension:
        // TODO: Case sensitive?
        if (file.find(".zip") == std::string::npos) {
            return false;
        }

        // File should exist:
        if (!getVFS()->exists(file)) {
            return false;
        }

        // File should start with the bytes "PK":
        // TODO: ...

        return true;
    }

    FIFE::VFSSource* ZipProvider::createSource(const std::string& file)
    {
        if (isReadable(file)) {
            VFSSource* source = nullptr;
            if (hasSource(file)) {
                source = m_sources[file];
            } else {
                source          = new ZipSource(getVFS(), file);
                m_sources[file] = source;
            }
            return source;
        } else {
            throw Exception("File " + file + " is not readable.");
        }
    }

    VFSSource* ZipProvider::getSource(const std::string& path) const
    {
        VFSSource* source = nullptr;
        if (hasSource(path)) {
            source = m_sources.find(path)->second;
        }
        return source;
    }

    bool ZipProvider::hasSource(const std::string& path) const
    {
        return m_sources.contains(path);
    }
} // namespace FIFE
