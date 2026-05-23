// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontfacecache.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

namespace FIFE
{

    bool FontFaceCache::has(AssetHandle handle) const
    {
        return m_cache.contains(handle.id);
    }

    std::shared_ptr<FontFace> FontFaceCache::get(AssetHandle handle) const
    {
        auto it = m_cache.find(handle.id);
        if (it == m_cache.end()) {
            throw std::out_of_range("FontFace not found in cache for handle: " + std::to_string(handle.id));
        }
        return it->second;
    }

    void FontFaceCache::put(AssetHandle handle, std::shared_ptr<FontFace> face)
    {
        if (!face) {
            throw std::invalid_argument("FontFace must not be null");
        }
        m_cache[handle.id] = std::move(face);
    }

    void FontFaceCache::remove(AssetHandle handle)
    {
        m_cache.erase(handle.id);
    }

    void FontFaceCache::clear()
    {
        m_cache.clear();
    }

    size_t FontFaceCache::size() const
    {
        return m_cache.size();
    }

} // namespace FIFE
