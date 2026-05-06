// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "textrenderpool.h"

// Standard C++ library includes
#include <algorithm>
#include <string>

// Platform specific includes

// 3rd party library includes

// FIFE includes
#include "fontbase.h"
#include "util/time/timemanager.h"
#include "video/image.h"

namespace FIFE
{

    TextRenderPool::TextRenderPool(size_t poolSize) : m_poolSize(0), m_poolMaxSize(poolSize)
    {
        m_collectTimer.setInterval(1000 * 60);
        m_collectTimer.setCallback([this] {
            removeOldEntries();
        });
    }

    TextRenderPool::~TextRenderPool()
    {
        auto it = m_pool.begin();
        for (; it != m_pool.end(); ++it) {
            delete it->image;
        }
    }

    Image* TextRenderPool::getRenderedText(FontBase const * fontbase, std::string const & text)
    {
        SDL_Color const c = fontbase->getColor();

        auto it = m_pool.begin();
        for (; it != m_pool.end(); ++it) {
            if (it->antialias != fontbase->isAntiAlias()) {
                continue;
            }

            if (it->glyph_spacing != fontbase->getGlyphSpacing()) {
                continue;
            }

            if (it->row_spacing != fontbase->getRowSpacing()) {
                continue;
            }

            if (it->color.r != c.r || it->color.g != c.g || it->color.b != c.b) {
                continue;
            }

            if (it->text != text) {
                continue;
            }

            // Stay sorted after access time
            it->timestamp = TimeManager::instance()->getTime();
            m_pool.push_front(*it);
            m_pool.erase(it);

            return m_pool.front().image;
        }
        return nullptr;
    }

    void TextRenderPool::addRenderedText(FontBase const * fontbase, std::string const & text, Image* image)
    {
        // Construct a entry and add it.
        s_pool_entry centry;
        centry.antialias     = fontbase->isAntiAlias();
        centry.glyph_spacing = fontbase->getGlyphSpacing();
        centry.row_spacing   = fontbase->getRowSpacing();
        centry.text          = text;
        centry.color         = fontbase->getColor();
        centry.image         = image;
        centry.timestamp     = TimeManager::instance()->getTime();
        m_pool.push_front(centry);

        // Some minimal amount of entries -> start collection timer
        // Don't have a timer active if only _some_ text is pooled.
        if (m_poolSize >= m_poolMaxSize / 10) {
            m_collectTimer.start();
        }

        // Maintain max pool size
        if (m_poolSize < m_poolMaxSize) {
            m_poolSize++;
            return;
        }
        delete m_pool.back().image;
        m_pool.pop_back();
    }

    void TextRenderPool::removeOldEntries()
    {

        auto it            = m_pool.begin();
        uint32_t const now = TimeManager::instance()->getTime();
        while (it != m_pool.end()) {
            if ((now - it->timestamp) > 1000 * 60) {
                delete it->image;
                it = m_pool.erase(it);
                --m_poolSize;
            } else {
                ++it;
            }
        }

        // Stop if nothing can grow old =)
        if (m_poolSize == 0) {
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
