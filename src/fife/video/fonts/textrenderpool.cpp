// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "textrenderpool.h"

// Standard C++ library includes
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "ifont.h"
#include "util/time/timemanager.h"
#include "video/image.h"

namespace FIFE
{

    TextRenderPool::s_pool_key TextRenderPool::makeKey(IFont const * font, std::string const & text)
    {
        s_pool_key key;
        key.text          = text;
        key.color         = font->getColor();
        key.antialias     = font->isAntiAlias();
        key.glyph_spacing = font->getGlyphSpacing();
        key.row_spacing   = font->getRowSpacing();
        return key;
    }

    TextRenderPool::s_pool_key TextRenderPool::makeKey(s_pool_entry const & entry)
    {
        s_pool_key key;
        key.text          = entry.text;
        key.color         = entry.color;
        key.antialias     = entry.antialias;
        key.glyph_spacing = entry.glyph_spacing;
        key.row_spacing   = entry.row_spacing;
        return key;
    }

    TextRenderPool::TextRenderPool(size_t poolSize) : m_poolMaxSize(poolSize)
    {
        m_collectTimer.setInterval(1000 * 60);
        m_collectTimer.setCallback([this] {
            removeOldEntries();
        });
    }

    TextRenderPool::~TextRenderPool() = default;

    Image* TextRenderPool::getRenderedText(IFont const * font, std::string const & text)
    {
        s_pool_key const key = makeKey(font, text);

        auto it = m_poolIndex.find(key);
        if (it == m_poolIndex.end()) {
            return nullptr;
        }

        // Stay sorted after access time
        it->second->timestamp = TimeManager::instance()->now64();
        m_pool.splice(m_pool.begin(), m_pool, it->second);

        return m_pool.front().image.get();
    }

    void TextRenderPool::addRenderedText(IFont const * font, std::string const & text, Image* image)
    {
        // Construct an entry and add it.
        s_pool_entry centry;
        centry.antialias     = font->isAntiAlias();
        centry.glyph_spacing = font->getGlyphSpacing();
        centry.row_spacing   = font->getRowSpacing();
        centry.text          = text;
        centry.color         = font->getColor();
        centry.image         = std::unique_ptr<Image>(image);
        centry.timestamp     = TimeManager::instance()->now64();

        s_pool_key const key = makeKey(font, text);

        // One list entry per map key: replace an existing entry instead of adding a duplicate.
        auto existing = m_poolIndex.find(key);
        if (existing != m_poolIndex.end()) {
            existing->second->image     = std::move(centry.image);
            existing->second->timestamp = centry.timestamp;
            m_pool.splice(m_pool.begin(), m_pool, existing->second);
            return;
        }

        // Transactional insert: list entry first, then the index.
        m_pool.push_front(std::move(centry));
        try {
            if (!m_poolIndex.emplace(key, m_pool.begin()).second) {
                // Defensive: duplicate key found between find and emplace.
                m_pool.erase(m_pool.begin());
                return;
            }
        } catch (...) {
            m_pool.erase(m_pool.begin());
            throw;
        }

        // Some minimal amount of entries -> start collection timer
        // Don't have a timer active if only _some_ text is pooled.
        if (m_pool.size() >= m_poolMaxSize / 10) {
            m_collectTimer.start();
        }

        // Maintain max pool size: evict least recently used entries beyond the limit.
        while (m_pool.size() > m_poolMaxSize) {
            auto last = std::prev(m_pool.end());
            m_poolIndex.erase(makeKey(*last));
            m_pool.erase(last);
        }
    }

    void TextRenderPool::removeOldEntries()
    {

        auto it            = m_pool.begin();
        uint64_t const now = TimeManager::instance()->now64();
        while (it != m_pool.end()) {
            if ((now - it->timestamp) > 1000ULL * 60) {
                m_poolIndex.erase(makeKey(*it));
                it = m_pool.erase(it);
            } else {
                ++it;
            }
        }

        // Stop if nothing can grow old =)
        if (m_pool.empty()) {
            m_collectTimer.stop();
        }
    }

    void TextRenderPool::invalidateCachedText()
    {
        auto it = m_pool.begin();
        while (it != m_pool.end()) {
            it->image->invalidate();
            ++it;
        }
    }
} // namespace FIFE
