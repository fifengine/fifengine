// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "video/fonts/assetresolver.h"

namespace FIFE
{

    class FilesystemAssetProvider : public AssetProvider
    {
        public:
            explicit FilesystemAssetProvider(std::vector<std::string> searchPaths);
            bool canResolve(AssetRequest const &) const override;
            AssetHandle resolve(AssetRequest const &) const override;
            int priority() const override
            {
                return 200;
            }

        private:
            std::vector<std::string> m_searchPaths;
            mutable uint64_t m_nextId = 1;
    };

} // namespace FIFE
