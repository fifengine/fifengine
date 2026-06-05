// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "map.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "cellcache.h"
#include "instance.h"
#include "layer.h"
#include "triggercontroller.h"
#include "util/base/exception.h"
#include "util/structures/purge.h"
#include "util/structures/rect.h"
#include "video/renderbackend.h"
#include "view/camera.h"
#include "view/rendererbase.h"

namespace FIFE
{

    Map::Map(
        std::string identifier,
        RenderBackend* renderBackend,
        std::vector<RendererBase*> const & renderers,
        TimeProvider* tp_master) :
        m_name(std::move(identifier)),

        m_timeProvider(tp_master),

        m_renderBackend(renderBackend),
        m_renderers(renderers),
        m_changed(false),
        m_triggerController(std::make_unique<TriggerController>(this))
    {
    }

    Map::~Map()
    {
        deleteLayers();
    }

    Layer* Map::getLayer(std::string const & id)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if ((*it)->getName() == id) {
                return it->get();
            }
        }
        return nullptr;
    }

    std::list<Layer*> Map::getLayers() const
    {
        std::list<Layer*> result;
        std::ranges::transform(m_layers, std::back_inserter(result), [](auto const & l) {
            return l.get();
        });
        return result;
    }

    uint32_t Map::getLayerCount() const
    {
        assert(std::cmp_less_equal(m_layers.size(), std::numeric_limits<uint32_t>::max()));
        return static_cast<uint32_t>(m_layers.size());
    }

    Layer* Map::createLayer(std::string const & identifier, CellGrid* grid)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if (identifier == (*it)->getName()) {
                throw NameClash(identifier);
            }
        }

        auto layer = std::make_unique<Layer>(identifier, this, grid);
        Layer* raw = layer.get();
        m_layers.push_back(std::move(layer));
        m_changed = true;
        auto i    = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            (*i)->onLayerCreate(this, raw);
            ++i;
        }

        return raw;
    }

    void Map::deleteLayer(Layer* layer)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if (it->get() == layer) {
                auto i = m_changeListeners.begin();
                while (i != m_changeListeners.end()) {
                    (*i)->onLayerDelete(this, layer);
                    ++i;
                }
                m_layers.erase(it);
                return;
            }
        }
        m_changed = true;
    }

    void Map::deleteLayers()
    {
        for (auto const & l : m_layers) {
            auto i = m_changeListeners.begin();
            while (i != m_changeListeners.end()) {
                (*i)->onLayerDelete(this, l.get());
                ++i;
            }
        }
        m_layers.clear();
    }

    void Map::getMinMaxCoordinates(ExactModelCoordinate& min, ExactModelCoordinate& max)
    {
        if (m_layers.empty()) {
            return;
        }
        auto it      = m_layers.begin();
        Layer* layer = it->get();
        for (; it != m_layers.end(); ++it) {
            ModelCoordinate newMin;
            ModelCoordinate newMax;
            (*it)->getMinMaxCoordinates(newMin, newMax, layer);

            min.x = std::min(min.x, static_cast<double>(newMin.x));
            max.x = std::max(max.x, static_cast<double>(newMax.x));
            min.y = std::min(min.y, static_cast<double>(newMin.y));
            max.y = std::max(max.y, static_cast<double>(newMax.y));
        }
        Location lmin(layer);
        Location lmax(layer);
        lmin.setExactLayerCoordinates(min);
        lmax.setExactLayerCoordinates(max);

        min = lmin.getMapCoordinates();
        max = lmax.getMapCoordinates();
    }

    bool Map::update()
    {
        m_changedLayers.clear();
        // transfer instances from one layer to another
        if (!m_transferInstances.empty()) {
            auto it = m_transferInstances.begin();
            for (; it != m_transferInstances.end(); ++it) {
                Instance* inst            = (*it).first;
                Location const target_loc = (*it).second;
                Layer* source             = inst->getOldLocationRef().getLayer();
                Layer* target             = target_loc.getLayer();
                if (source != target) {
                    source->removeInstance(inst);
                    target->addInstance(inst, target_loc.getExactLayerCoordinates());
                }
            }
            m_transferInstances.clear();
        }
        std::vector<CellCache*> cellCaches;
        auto it = m_layers.begin();
        // update Layers
        for (; it != m_layers.end(); ++it) {
            if ((*it)->update()) {
                m_changedLayers.push_back(it->get());
            }
            CellCache* cache = (*it)->getCellCache();
            if (cache != nullptr) {
                cellCaches.push_back(cache);
            }
        }
        // loop over Caches and update
        for (auto& cellCache : cellCaches) {
            cellCache->update();
        }
        if (!m_changedLayers.empty()) {
            auto i = m_changeListeners.begin();
            while (i != m_changeListeners.end()) {
                (*i)->onMapChanged(this, m_changedLayers);
                ++i;
            }
        }

        // loop over cameras and update if enabled
        auto camIter = m_cameras.begin();
        for (; camIter != m_cameras.end(); ++camIter) {
            if ((*camIter)->isEnabled()) {
                (*camIter)->update();
                (*camIter)->render();
            }
        }

        bool const retval = m_changed;
        m_changed         = false;
        return retval;
    }

    void Map::addChangeListener(MapChangeListener* listener)
    {
        m_changeListeners.push_back(listener);
    }

    void Map::removeChangeListener(MapChangeListener const * listener)
    {
        auto i = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            if ((*i) == listener) {
                m_changeListeners.erase(i);
                return;
            }
            ++i;
        }
    }

    Camera* Map::addCamera(std::string const & id, Rect const & viewport)
    {
        if (getCamera(id) != nullptr) {
            std::string const errorStr = "Camera: " + id + " already exists";
            throw NameClash(errorStr);
        }

        // create new camera and add to list of cameras
        auto camera = std::make_unique<Camera>(id, this, viewport, m_renderBackend);
        Camera* raw = camera.get();

        auto iter = m_renderers.begin();
        for (; iter != m_renderers.end(); ++iter) {
            raw->addRenderer((*iter)->clone());
        }
        m_cameras.push_back(std::move(camera));
        return raw;
    }

    void Map::removeCamera(std::string const & id)
    {
        auto iter = m_cameras.begin();
        for (; iter != m_cameras.end(); ++iter) {
            if ((*iter)->getName() == id) {
                m_cameras.erase(iter);
                break;
            }
        }
    }

    Camera* Map::getCamera(std::string const & id)
    {
        auto iter = m_cameras.begin();
        for (; iter != m_cameras.end(); ++iter) {
            if ((*iter)->getName() == id) {
                return iter->get();
            }
        }

        return nullptr;
    }

    std::vector<std::unique_ptr<Camera>> const & Map::getCameras() const
    {
        return m_cameras;
    }

    uint32_t Map::getActiveCameraCount() const
    {
        uint32_t count = 0;
        auto it        = m_cameras.begin();
        for (; it != m_cameras.end(); ++it) {
            if ((*it)->isEnabled()) {
                count += 1;
            }
        }
        return count;
    }

    void Map::addInstanceForTransfer(Instance* instance, Location const & target)
    {
        std::pair<std::map<Instance*, Location>::iterator, bool> const insertiter =
            m_transferInstances.insert(std::make_pair(instance, target));
        if (!insertiter.second) {
            Location& loc = insertiter.first->second;
            loc.setLayer(target.getLayer());
            loc.setExactLayerCoordinates(target.getExactLayerCoordinates());
        }
    }

    void Map::removeInstanceForTransfer(Instance* instance)
    {
        auto it = m_transferInstances.find(instance);
        if (it != m_transferInstances.end()) {
            m_transferInstances.erase(it);
        }
    }

    void Map::initializeCellCaches()
    {
        if (m_layers.empty()) {
            return;
        }

        auto layit = m_layers.begin();
        // first add interacts to walkables
        for (; layit != m_layers.end(); ++layit) {
            if ((*layit)->isInteract()) {
                Layer* temp = getLayer((*layit)->getWalkableId());
                if (temp != nullptr) {
                    temp->addInteractLayer(layit->get());
                }
            }
        }
        // then create CellCaches for walkables
        layit = m_layers.begin();
        for (; layit != m_layers.end(); ++layit) {
            if ((*layit)->isWalkable()) {
                (*layit)->createCellCache();
            }
        }
    }

    void Map::finalizeCellCaches()
    {
        // create Cells and generate neighbours
        auto layit = m_layers.begin();
        for (; layit != m_layers.end(); ++layit) {
            CellCache* cache = (*layit)->getCellCache();
            if (cache != nullptr) {
                cache->createCells();
                cache->forceUpdate();
            }
        }
    }
} // namespace FIFE
