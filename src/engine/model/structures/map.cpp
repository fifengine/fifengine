// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/structures/purge.h"
#include "util/structures/rect.h"
#include "video/renderbackend.h"
#include "view/camera.h"
#include "view/rendererbase.h"

#include "cellcache.h"
#include "instance.h"
#include "layer.h"
#include "map.h"
#include "triggercontroller.h"

namespace FIFE
{

    Map::Map(
        std::string identifier,
        RenderBackend* renderBackend,
        const std::vector<RendererBase*>& renderers,
        TimeProvider* tp_master) :
        m_id(std::move(identifier)),

        m_timeProvider(tp_master),

        m_renderBackend(renderBackend),
        m_renderers(renderers),
        m_changed(false),
        m_triggerController(new TriggerController(this))
    {
    }

    Map::~Map()
    {
        delete m_triggerController;
        // remove all cameras
        auto iter = m_cameras.begin();
        for (; iter != m_cameras.end(); ++iter) {
            delete *iter;
        }
        m_cameras.clear();

        deleteLayers();
    }

    Layer* Map::getLayer(const std::string& id)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if ((*it)->getId() == id) {
                return *it;
            }
        }
        return nullptr;
    }

    uint32_t Map::getLayerCount() const
    {
        return m_layers.size();
    }

    Layer* Map::createLayer(const std::string& identifier, CellGrid* grid)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if (identifier == (*it)->getId()) {
                throw NameClash(identifier);
            }
        }

        auto* layer = new Layer(identifier, this, grid);
        m_layers.push_back(layer);
        m_changed = true;
        auto i    = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            (*i)->onLayerCreate(this, layer);
            ++i;
        }

        return layer;
    }

    void Map::deleteLayer(Layer* layer)
    {
        auto it = m_layers.begin();
        for (; it != m_layers.end(); ++it) {
            if ((*it) == layer) {
                auto i = m_changeListeners.begin();
                while (i != m_changeListeners.end()) {
                    (*i)->onLayerDelete(this, layer);
                    ++i;
                }
                delete layer;
                m_layers.erase(it);
                return;
            }
        }
        m_changed = true;
    }

    void Map::deleteLayers()
    {
        std::list<Layer*> temp_layers = m_layers;
        auto temp_it                  = temp_layers.begin();
        for (; temp_it != temp_layers.end(); ++temp_it) {
            auto i = m_changeListeners.begin();
            while (i != m_changeListeners.end()) {
                (*i)->onLayerDelete(this, *temp_it);
                ++i;
            }
            auto it = m_layers.begin();
            for (; it != m_layers.end(); ++it) {
                if (*it == *temp_it) {
                    delete *it;
                    m_layers.erase(it);
                    break;
                }
            }
        }
    }

    void Map::getMinMaxCoordinates(ExactModelCoordinate& min, ExactModelCoordinate& max)
    {
        if (m_layers.empty()) {
            return;
        }
        auto it      = m_layers.begin();
        Layer* layer = *it;
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
                const Location target_loc = (*it).second;
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
                m_changedLayers.push_back(*it);
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

        const bool retval = m_changed;
        m_changed         = false;
        return retval;
    }

    void Map::addChangeListener(MapChangeListener* listener)
    {
        m_changeListeners.push_back(listener);
    }

    void Map::removeChangeListener(MapChangeListener* listener)
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

    Camera* Map::addCamera(const std::string& id, const Rect& viewport)
    {
        if (getCamera(id) != nullptr) {
            const std::string errorStr = "Camera: " + id + " already exists";
            throw NameClash(errorStr);
        }

        // create new camera and add to list of cameras
        auto* camera = new Camera(id, this, viewport, m_renderBackend);
        m_cameras.push_back(camera);

        auto iter = m_renderers.begin();
        for (; iter != m_renderers.end(); ++iter) {
            camera->addRenderer((*iter)->clone());
        }

        return camera;
    }

    void Map::removeCamera(const std::string& id)
    {
        auto iter = m_cameras.begin();
        for (; iter != m_cameras.end(); ++iter) {
            if ((*iter)->getId() == id) {
                // camera has been found delete it
                delete *iter;

                // now remove it from the vector
                // note this invalidates iterators, but we do not need
                // to worry about it in this case since we are done
                m_cameras.erase(iter);

                break;
            }
        }
    }

    Camera* Map::getCamera(const std::string& id)
    {
        auto iter = m_cameras.begin();
        for (; iter != m_cameras.end(); ++iter) {
            if ((*iter)->getId() == id) {
                return *iter;
            }
        }

        return nullptr;
    }

    const std::vector<Camera*>& Map::getCameras() const
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

    void Map::addInstanceForTransfer(Instance* instance, const Location& target)
    {
        const std::pair<std::map<Instance*, Location>::iterator, bool> insertiter =
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
                    temp->addInteractLayer(*layit);
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
