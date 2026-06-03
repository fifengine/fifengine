// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "filesystemassetprovider.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
namespace fs = std::filesystem;

namespace FIFE
{

    FilesystemAssetProvider::FilesystemAssetProvider(std::vector<std::string> searchPaths) :
        m_searchPaths(std::move(searchPaths))
    {
    }

    bool FilesystemAssetProvider::canResolve(AssetRequest const & req) const
    {
        assert("Asset request source must not be empty" && !req.source.empty());
        if (std::ranges::any_of(m_searchPaths, [&](auto const & path) {
                return fs::exists(fs::path(path) / req.source);
            })) {
            return true;
        }
        auto const direct = fs::path(req.source);
        return direct.is_absolute() && fs::exists(direct);
    }

    AssetHandle FilesystemAssetProvider::resolve(AssetRequest const & req) const
    {
        assert("Asset request source must not be empty" && !req.source.empty());
        return AssetHandle{m_nextId++};
    }

} // namespace FIFE
