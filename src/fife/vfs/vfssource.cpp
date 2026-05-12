// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "vfssource.h"

// Standard C++ library includes
#include <algorithm>
#include <string>

// 3rd party library includes

// FIFE includes
#include "vfs.h"

namespace FIFE
{

    VFSSource::VFSSource(VFS* vfs) : m_vfs(vfs)
    {
    }

    VFSSource::~VFSSource()
    {
        m_vfs->removeSource(this);
    }

} // namespace FIFE

std::string FIFE::VFSSource::fixPath(std::string path) const
{
    if (path.empty()) {
        return path;
    }

    size_t pos = 0;
    while ((pos = path.find('\\')) != std::string::npos) {
        path[pos] = '/';
    }

    if (path[0] == '/') {
        path = path.substr(1, path.length());
    }

    return path;
}
