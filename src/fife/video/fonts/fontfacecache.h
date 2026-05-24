// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#pragma once

#include <memory>
#include <unordered_map>

#include "fontface.h"
#include "fonttypes.h"
#include "platform.h"

namespace FIFE
{

    class FIFE_API FontFaceCache
    {
        public:
            FontFaceCache()  = default;
            ~FontFaceCache() = default;

            bool has(FontFaceKey key) const;
            std::shared_ptr<FontFace> get(FontFaceKey key) const;
            void put(FontFaceKey key, std::shared_ptr<FontFace> face);
            void remove(FontFaceKey key);
            void clear();
            size_t size() const;

        private:
            std::unordered_map<FontFaceKey, std::shared_ptr<FontFace>> m_cache;
    };

} // namespace FIFE
