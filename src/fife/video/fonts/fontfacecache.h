// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>
#include <unordered_map>

#include "fontface.h"
#include "fonttypes.h"

namespace FIFE
{

    class FontFaceCache
    {
        public:
            FontFaceCache()  = default;
            ~FontFaceCache() = default;

            bool has(AssetHandle handle) const;
            std::shared_ptr<FontFace> get(AssetHandle handle) const;
            void put(AssetHandle handle, std::shared_ptr<FontFace> face);
            void remove(AssetHandle handle);
            void clear();
            size_t size() const;

        private:
            std::unordered_map<uint64_t, std::shared_ptr<FontFace>> m_cache;
    };

} // namespace FIFE
