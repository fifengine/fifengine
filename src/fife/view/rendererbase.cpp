// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "rendererbase.h"

// Standard C++ library includes
#include <algorithm>
#include <list>

// 3rd party library includes

// FIFE includes
#include "model/structures/layer.h"
#include "model/structures/map.h"
#include "util/log/logger.h"

namespace FIFE
{
    static Logger _log(LM_VIEW);

    // use some big value, so that non-positioned renderers show on top
    int32_t const DEFAULT_RENDERER_POSITION = 1000;

    RendererBase::RendererBase(RenderBackend* renderbackend, int32_t position) :
        m_renderbackend(renderbackend),
        m_enabled(false),
        m_pipeline_position(DEFAULT_RENDERER_POSITION),
        m_listener(nullptr)
    {
        setPipelinePosition(position);
    }

    RendererBase::RendererBase(RendererBase const & old) :
        m_renderbackend(old.m_renderbackend),
        m_enabled(old.m_enabled),
        m_pipeline_position(old.m_pipeline_position),
        m_listener(nullptr)
    {
        setPipelinePosition(old.m_pipeline_position);
    }

    RendererBase::RendererBase() :
        m_renderbackend(nullptr), m_enabled(false), m_pipeline_position(DEFAULT_RENDERER_POSITION), m_listener(nullptr)
    {
    }

    void RendererBase::setPipelinePosition(int32_t position)
    {
        if (position != m_pipeline_position) {
            m_pipeline_position = position;
            if (m_listener != nullptr) {
                m_listener->onRendererPipelinePositionChanged(this);
            }
        }
    }

    void RendererBase::setEnabled(bool enabled)
    {
        if (m_enabled != enabled) {
            m_enabled = enabled;
            if (m_listener != nullptr) {
                m_listener->onRendererEnabledChanged(this);
            }
        }
    }

    void RendererBase::addActiveLayer(Layer* layer)
    {
        if (std::ranges::find(m_active_layers, layer) == m_active_layers.end()) {
            m_active_layers.push_back(layer);
        }
    }

    void RendererBase::removeActiveLayer(Layer* layer)
    {
        m_active_layers.remove(layer);
    }

    void RendererBase::clearActiveLayers()
    {
        m_active_layers.clear();
    }

    bool RendererBase::isActivedLayer(Layer* layer)
    {
        return std::ranges::find(m_active_layers, layer) != m_active_layers.end();
    }

    void RendererBase::activateAllLayers(Map* map)
    {
        clearActiveLayers();

        std::list<Layer*> const & tmp = map->getLayers();
        auto it                       = tmp.begin();
        for (; it != tmp.end(); ++it) {
            addActiveLayer(*it);
        }
    }

} // namespace FIFE
