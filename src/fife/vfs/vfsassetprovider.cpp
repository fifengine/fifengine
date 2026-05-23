// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "vfsassetprovider.h"

#include <cassert>

namespace FIFE
{

    VfsAssetProvider::VfsAssetProvider(VFS* vfs) : m_vfs(vfs)
    {
        assert("VFS must not be null" && m_vfs != nullptr);
    }

    bool VfsAssetProvider::canResolve(AssetRequest const & req) const
    {
        assert("Asset request source must not be empty" && !req.source.empty());
        return m_vfs->exists(req.source);
    }

    AssetHandle VfsAssetProvider::resolve(AssetRequest const & req) const
    {
        assert("Asset request source must not be empty" && !req.source.empty());
        return AssetHandle{m_nextId++};
    }

} // namespace FIFE
