// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "fontfacecache.h"

#include <cassert>
#include <memory>
#include <stdexcept>
#include <utility>

namespace FIFE
{

    bool FontFaceCache::has(FontFaceKey key) const
    {
        return m_cache.contains(key);
    }

    std::shared_ptr<FontFace> FontFaceCache::get(FontFaceKey key) const
    {
        auto it = m_cache.find(key);
        if (it == m_cache.end()) {
            throw std::out_of_range(
                "FontFace not found in cache for asset " + std::to_string(key.asset.id) + " ptsize " +
                std::to_string(key.ptsize));
        }
        return it->second;
    }

    void FontFaceCache::put(FontFaceKey key, std::shared_ptr<FontFace> face)
    {
        if (!face) {
            throw std::invalid_argument("FontFace must not be null");
        }
        m_cache[key] = std::move(face);
    }

    void FontFaceCache::remove(FontFaceKey key)
    {
        m_cache.erase(key);
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
