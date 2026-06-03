// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once
#include <memory>
#include <vector>

#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API AssetProvider
    {
        public:
            virtual ~AssetProvider()                                = default;
            virtual bool canResolve(AssetRequest const &) const     = 0;
            virtual AssetHandle resolve(AssetRequest const &) const = 0;
            virtual int priority() const
            {
                return 0;
            }
    };

    class FIFE_API AssetResolver
    {
        public:
            AssetResolver()                                 = default;
            AssetResolver(AssetResolver&&)                  = default;
            AssetResolver& operator=(AssetResolver&&)       = default;
            AssetResolver(AssetResolver const &)            = delete;
            AssetResolver& operator=(AssetResolver const &) = delete;

            void addProvider(std::unique_ptr<AssetProvider> provider);
            AssetHandle resolve(AssetRequest const & request) const;

        private:
            std::vector<std::unique_ptr<AssetProvider>> m_providers;
    };

} // namespace FIFE
