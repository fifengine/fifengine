// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once
#include <cstdint>

#include "vfs.h"
#include "video/fonts/assetresolver.h"

namespace FIFE
{

    class VfsAssetProvider : public AssetProvider
    {
        public:
            explicit VfsAssetProvider(VFS* vfs);
            bool canResolve(AssetRequest const &) const override;
            AssetHandle resolve(AssetRequest const &) const override;
            int priority() const override
            {
                return 100;
            }

        private:
            VFS* m_vfs;
            mutable uint64_t m_nextId = 1;
    };

} // namespace FIFE
