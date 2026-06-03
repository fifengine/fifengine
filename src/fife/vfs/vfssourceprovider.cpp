// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "vfssourceprovider.h"

// Standard C++ library includes
#include <string>
#include <utility>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"
#include "vfssource.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VFS);
            return log;
        }
    } // namespace

    VFSSourceProvider::VFSSourceProvider(std::string name) : m_vfs(nullptr), m_name(std::move(name))
    {
    }

    VFSSourceProvider::~VFSSourceProvider() = default;

    void VFSSourceProvider::setVFS(VFS* vfs)
    {
        if (m_vfs != nullptr) {
            FL_WARN(_log(), "Attempt to set a VFSSourceProvider that is already associated with a VFS.");
            return;
        }
        m_vfs = vfs;
    }

    std::string const & VFSSourceProvider::getName() const
    {
        return m_name;
    }
} // namespace FIFE
