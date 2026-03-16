// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "vfs.h"
#include "vfssource.h"

namespace FIFE
{

    VFSSource::VFSSource(VFS* vfs) : m_vfs(vfs) { }

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
