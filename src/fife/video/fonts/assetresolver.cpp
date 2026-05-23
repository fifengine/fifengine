// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "assetresolver.h"

#include <algorithm>
#include <cassert>
#include <utility>

namespace FIFE
{

    void AssetResolver::addProvider(std::unique_ptr<AssetProvider> provider)
    {
        assert("Provider must not be null" && provider != nullptr);
        m_providers.push_back(std::move(provider));
        std::ranges::sort(m_providers, [](auto const & a, auto const & b) {
            return a->priority() < b->priority();
        });
    }

    AssetHandle AssetResolver::resolve(AssetRequest const & request) const
    {
        auto it = std::ranges::find_if(m_providers, [&](auto const & p) {
            return p->canResolve(request);
        });
        if (it != m_providers.end()) {
            return (*it)->resolve(request);
        }
        return AssetHandle{};
    }

} // namespace FIFE
