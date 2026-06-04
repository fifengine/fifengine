// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "model.h"

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/ipather.h"
#include "model/metamodel/object.h"
#include "structures/instance.h"
#include "structures/layer.h"
#include "structures/map.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/structures/purge.h"
#include "video/renderbackend.h"
#include "view/rendererbase.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_MODEL);
            return log;
        }
    } // namespace

    class ModelMapObserver : public MapChangeListener
    {
            Model* m_model;

        public:
            explicit ModelMapObserver(Model* model) : m_model(model)
            {
            }
            ~ModelMapObserver() override                          = default;
            ModelMapObserver(ModelMapObserver const &)            = delete;
            ModelMapObserver& operator=(ModelMapObserver const &) = delete;
            ModelMapObserver(ModelMapObserver&&)                  = delete;
            ModelMapObserver& operator=(ModelMapObserver&&)       = delete;

            void onMapChanged([[maybe_unused]] Map* map, [[maybe_unused]] std::vector<Layer*>& changedLayers) override
            {
            }

            void onLayerCreate([[maybe_unused]] Map* map, [[maybe_unused]] Layer* layer) override
            {
            }

            void onLayerDelete([[maybe_unused]] Map* map, Layer* layer) override
            {
                m_model->removeCellGrid(layer->getCellGrid());
            }
    };

    Model::Model(RenderBackend* renderbackend, std::vector<std::unique_ptr<RendererBase>> const & renderers) :
        m_mapObserver(std::make_unique<ModelMapObserver>(this)),
        m_lastNamespace(nullptr),
        m_timeprovider(nullptr),
        m_renderbackend(renderbackend)
    {
        m_renderers.reserve(renderers.size());
        std::ranges::transform(renderers, std::back_inserter(m_renderers), [](auto const & r) {
            return r.get();
        });
    }

    Model::~Model()
    {
        // first remove the map observer, we delete all grids anyway
        for (auto& m_map : m_maps) {
            m_map->removeChangeListener(m_mapObserver.get());
        }
    }

    Map* Model::createMap(std::string const & identifier)
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if (identifier == (*it)->getName()) {
                throw NameClash(identifier);
            }
        }

        auto map = std::make_unique<Map>(identifier, m_renderbackend, m_renderers, &m_timeprovider);
        map->addChangeListener(m_mapObserver.get());
        Map* ptr = map.get();
        m_maps.push_back(std::move(map));
        return ptr;
    }

    void Model::adoptPather(std::unique_ptr<IPather> pather)
    {
        m_pathers.push_back(std::move(pather));
    }

    IPather* Model::getPather(std::string const & pathername)
    {
        auto it = m_pathers.begin();
        for (; it != m_pathers.end(); ++it) {
            if ((*it)->getName() == pathername) {
                return it->get();
            }
        }
        FL_WARN(_log(), "No pather of requested type \"" + pathername + "\" found.");
        return nullptr;
    }

    void Model::adoptCellGrid(std::unique_ptr<CellGrid> grid)
    {
        m_adoptedGrids.push_back(std::move(grid));
    }

    CellGrid* Model::getCellGrid(std::string const & gridtype)
    {
        auto it = m_adoptedGrids.begin();
        for (; it != m_adoptedGrids.end(); ++it) {
            if ((*it)->getType() == gridtype) {
                auto newcg    = std::unique_ptr<CellGrid>((*it)->clone());
                CellGrid* ptr = newcg.get();
                m_createdGrids.push_back(std::move(newcg));
                return ptr;
            }
        }
        FL_WARN(_log(), "No cellgrid of requested type \"" + gridtype + "\" found.");
        return nullptr;
    }

    void Model::removeCellGrid(CellGrid* grid)
    {
        if (grid == nullptr) {
            return;
        }

        auto it = std::ranges::find_if(m_createdGrids, [grid](auto const & ptr) {
            return ptr.get() == grid;
        });
        if (it != m_createdGrids.end()) {
            m_createdGrids.erase(it);
            return;
        }
    }

    Map* Model::getMap(std::string const & identifier) const
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if ((*it)->getName() == identifier) {
                return it->get();
            }
        }

        throw NotFound(std::string("Tried to get non-existent map: ") + identifier + ".");
    }

    void Model::deleteMap(Map const * map)
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if (it->get() == map) {
                m_maps.erase(it);
                return;
            }
        }
    }

    uint32_t Model::getMapCount() const
    {
        assert(std::cmp_less_equal(m_maps.size(), std::numeric_limits<uint32_t>::max()));
        return static_cast<uint32_t>(m_maps.size());
    }

    void Model::deleteMaps()
    {
        // first remove the map observer, we delete all grids anyway
        for (auto& m_map : m_maps) {
            m_map->removeChangeListener(m_mapObserver.get());
        }
        m_maps.clear();
        m_createdGrids.clear();
    }

    uint32_t Model::getActiveCameraCount() const
    {
        uint32_t count = 0;
        auto it        = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            count += (*it)->getActiveCameraCount();
        }
        return count;
    }

    std::list<std::string> Model::getNamespaces() const
    {
        std::list<std::string> namespace_list;
        auto nspace = m_namespaces.begin();
        for (; nspace != m_namespaces.end(); ++nspace) {
            namespace_list.push_back(nspace->first);
        }
        return namespace_list;
    }

    Object* Model::createObject(std::string const & identifier, std::string const & name_space, Object* parent)
    {
        // Find or create namespace
        namespace_t* nspace = selectNamespace(name_space);
        if (nspace == nullptr) {
            m_namespaces.emplace_back(name_space, objectmap_t());
            nspace = selectNamespace(name_space);
        }

        // Check for nameclashes
        auto it = nspace->second.find(identifier);
        if (it != nspace->second.end()) {
            throw NameClash(identifier);
        }

        // Finally insert & create
        auto object                = std::make_unique<Object>(identifier, name_space, parent);
        Object* ptr                = object.get();
        nspace->second[identifier] = std::move(object);
        return ptr;
    }

    bool Model::deleteObject(Object* object)
    {
        // WARNING: This code has obviously not been tested (thoroughly).

        // Check if any instances exist. If yes - bail out.
        for (auto const & m_map : m_maps) {
            auto layers = m_map->getLayers();
            for (auto const * layer : layers) {
                for (auto const & kt : layer->getInstances()) {
                    Object const * o = kt->getObject();
                    if (o == object) {
                        return false;
                    }
                }
            }
        }

        // Check if the namespace exists
        namespace_t* nspace = selectNamespace(object->getNamespace());
        if (nspace == nullptr) {
            return true;
        }

        // If yes - delete+erase object.
        auto it = nspace->second.find(object->getName());
        if (it != nspace->second.end()) {
            nspace->second.erase(it);
        }

        return true;
    }

    bool Model::deleteObjects()
    {
        // If we have layers with instances - bail out.
        for (auto const & m_map : m_maps) {
            auto layers = m_map->getLayers();
            auto jt     = std::ranges::find_if(layers, [](Layer const * layer) {
                return layer->hasInstances();
            });
            if (jt != layers.end()) {
                return false;
            }
        }

        // Otherwise delete every object in every namespace
        m_namespaces.clear();
        m_lastNamespace = nullptr;
        return true;
    }

    Object* Model::getObject(std::string const & id, std::string const & name_space)
    {
        namespace_t* nspace = selectNamespace(name_space);
        if (nspace != nullptr) {
            auto it = nspace->second.find(id);
            if (it != nspace->second.end()) {
                return it->second.get();
            }
        }
        return nullptr;
    }

    std::list<Object*> Model::getObjects(std::string const & name_space) const
    {
        std::list<Object*> object_list;
        namespace_t const * nspace = selectNamespace(name_space);
        if (nspace != nullptr) {
            auto it = nspace->second.begin();
            for (; it != nspace->second.end(); ++it) {
                object_list.push_back(it->second.get());
            }
        }

        return object_list;
    }

    Model::namespace_t const * Model::selectNamespace(std::string const & name_space) const
    {
        auto nspace = m_namespaces.begin();
        for (; nspace != m_namespaces.end(); ++nspace) {
            if (nspace->first == name_space) {
                return &(*nspace);
            }
        }

        return nullptr;
    }

    Model::namespace_t* Model::selectNamespace(std::string const & name_space)
    {
        if ((m_lastNamespace != nullptr) && m_lastNamespace->first == name_space) {
            return m_lastNamespace;
        }
        auto nspace = m_namespaces.begin();
        for (; nspace != m_namespaces.end(); ++nspace) {
            if (nspace->first == name_space) {
                m_lastNamespace = &(*nspace);
                return m_lastNamespace;
            }
        }
        m_lastNamespace = nullptr;
        return nullptr;
    }

    void Model::update()
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            (*it)->update();
        }
        auto jt = m_pathers.begin();
        for (; jt != m_pathers.end(); ++jt) {
            (*jt)->update();
        }
    }

} // namespace FIFE
