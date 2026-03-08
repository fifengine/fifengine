// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

#include "vfssource.h"
#include "vfssourceprovider.h"

namespace FIFE
{
    /** Logger to use for this source file.
     *  @relates Logger
     */
    static Logger _log(LM_VFS);

    VFSSourceProvider::VFSSourceProvider(const std::string& name) : m_vfs(0), m_name(name) { }

    VFSSourceProvider::~VFSSourceProvider() { }

    void VFSSourceProvider::setVFS(VFS* vfs)
    {
        if (m_vfs) {
            FL_WARN(_log, "Attempt to set a VFSSourceProvider that is already associated with a VFS.");
            return;
        }
        m_vfs = vfs;
    }

    const std::string& VFSSourceProvider::getName() const
    {
        return m_name;
    }
} // namespace FIFE