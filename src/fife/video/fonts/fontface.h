// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>

#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API FontFace
    {
        public:
            explicit FontFace(AssetHandle handle);
            virtual ~FontFace() = default;
            AssetHandle getAssetHandle() const
            {
                return m_assetHandle;
            }
            virtual bool supports(uint32_t codepoint) const;

        protected:
            AssetHandle m_assetHandle;
    };

} // namespace FIFE
