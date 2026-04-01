// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <limits>
#include <list>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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

#include "model.h"

namespace FIFE
{
    static Logger _log(LM_MODEL);

    class ModelMapObserver : public MapChangeListener
    {
        Model* m_model;

    public:
        explicit ModelMapObserver(Model* model) : m_model(model) { }
        ~ModelMapObserver() override = default;

        void onMapChanged(Map* map, std::vector<Layer*>& changedLayers) override { }

        void onLayerCreate(Map* map, Layer* layer) override { }

        void onLayerDelete(Map* map, Layer* layer) override
        {
            m_model->removeCellGrid(layer->getCellGrid());
        }
    };

    Model::Model(RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers) :

        m_lastNamespace(nullptr),
        m_timeprovider(nullptr),
        m_renderbackend(renderbackend),
        m_renderers(renderers),
        m_mapObserver(new ModelMapObserver(this))
    {
    }

    Model::~Model()
    {
        // first remove the map observer, we delete all grids anyway
        for (auto& m_map : m_maps) {
            m_map->removeChangeListener(m_mapObserver);
            delete m_map;
        }
        delete m_mapObserver;

        for (auto& m_namespace : m_namespaces) {
            purge_map(m_namespace.second);
        }
        purge(m_pathers);
        purge(m_createdGrids);
        purge(m_adoptedGrids);
    }

    Map* Model::createMap(const std::string& identifier)
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if (identifier == (*it)->getId()) {
                throw NameClash(identifier);
            }
        }

        Map* map = new Map(identifier, m_renderbackend, m_renderers, &m_timeprovider);
        map->addChangeListener(m_mapObserver);
        m_maps.push_back(map);
        return map;
    }

    void Model::adoptPather(IPather* pather)
    {
        m_pathers.push_back(pather);
    }

    IPather* Model::getPather(const std::string& pathername)
    {
        auto it = m_pathers.begin();
        for (; it != m_pathers.end(); ++it) {
            if ((*it)->getName() == pathername) {
                return *it;
            }
        }
        FL_WARN(_log, "No pather of requested type \"" + pathername + "\" found.");
        return nullptr;
    }

    void Model::adoptCellGrid(CellGrid* grid)
    {
        m_adoptedGrids.push_back(grid);
    }

    CellGrid* Model::getCellGrid(const std::string& gridtype)
    {
        auto it = m_adoptedGrids.begin();
        for (; it != m_adoptedGrids.end(); ++it) {
            if ((*it)->getType() == gridtype) {
                CellGrid* newcg = (*it)->clone();
                m_createdGrids.push_back(newcg);
                return newcg;
            }
        }
        FL_WARN(_log, "No cellgrid of requested type \"" + gridtype + "\" found.");
        return nullptr;
    }

    void Model::removeCellGrid(CellGrid* grid)
    {
        if (grid == nullptr) {
            return;
        }

        auto it = std::ranges::find(m_createdGrids, grid);
        if (it != m_createdGrids.end()) {
            delete *it;
            m_createdGrids.erase(it);
            return;
        }
    }

    Map* Model::getMap(const std::string& identifier) const
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if ((*it)->getId() == identifier) {
                return *it;
            }
        }

        throw NotFound(std::string("Tried to get non-existent map: ") + identifier + ".");
    }

    void Model::deleteMap(Map* map)
    {
        auto it = m_maps.begin();
        for (; it != m_maps.end(); ++it) {
            if (*it == map) {
                delete *it;
                m_maps.erase(it);
                return;
            }
        }
    }

    uint32_t Model::getMapCount() const
    {
        assert(m_maps.size() <= std::numeric_limits<uint32_t>::max());
        return static_cast<uint32_t>(m_maps.size());
    }

    void Model::deleteMaps()
    {
        // first remove the map observer, we delete all grids anyway
        for (auto& m_map : m_maps) {
            m_map->removeChangeListener(m_mapObserver);
            delete m_map;
        }
        m_maps.clear();
        purge(m_createdGrids);
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

    Object* Model::createObject(const std::string& identifier, const std::string& name_space, Object* parent)
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
        auto* object               = new Object(identifier, name_space, parent);
        nspace->second[identifier] = object;
        return object;
    }

    bool Model::deleteObject(Object* object)
    {
        // WARNING: This code has obviously not been tested (thoroughly).

        // Check if any instances exist. If yes - bail out.
        std::list<Layer*>::const_iterator jt;
        std::vector<Instance*>::const_iterator kt;
        for (auto& m_map : m_maps) {
            for (jt = m_map->getLayers().begin(); jt != m_map->getLayers().end(); ++jt) {
                for (kt = (*jt)->getInstances().begin(); kt != (*jt)->getInstances().end(); ++kt) {
                    const Object* o = (*kt)->getObject();
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
        auto it = nspace->second.find(object->getId());
        if (it != nspace->second.end()) {
            delete it->second;
            nspace->second.erase(it);
        }

        return true;
    }

    bool Model::deleteObjects()
    {
        // If we have layers with instances - bail out.
        std::list<Layer*>::const_iterator jt;
        for (auto& m_map : m_maps) {
            jt = std::find_if(m_map->getLayers().begin(), m_map->getLayers().end(), [](const Layer* layer) {
                return layer->hasInstances();
            });
            if (jt != m_map->getLayers().end()) {
                return false;
            }
        }

        // Otherwise delete every object in every namespace
        auto nspace = m_namespaces.begin();
        while (nspace != m_namespaces.end()) {
            auto it = nspace->second.begin();
            for (; it != nspace->second.end(); ++it) {
                delete it->second;
            }
            nspace = m_namespaces.erase(nspace);
        }
        m_lastNamespace = nullptr;
        return true;
    }

    Object* Model::getObject(const std::string& id, const std::string& name_space)
    {
        namespace_t* nspace = selectNamespace(name_space);
        if (nspace != nullptr) {
            auto it = nspace->second.find(id);
            if (it != nspace->second.end()) {
                return it->second;
            }
        }
        return nullptr;
    }

    std::list<Object*> Model::getObjects(const std::string& name_space) const
    {
        std::list<Object*> object_list;
        const namespace_t* nspace = selectNamespace(name_space);
        if (nspace != nullptr) {
            auto it = nspace->second.begin();
            for (; it != nspace->second.end(); ++it) {
                object_list.push_back(it->second);
            }
        }

        return object_list;
    }

    const Model::namespace_t* Model::selectNamespace(const std::string& name_space) const
    {
        auto nspace = m_namespaces.begin();
        for (; nspace != m_namespaces.end(); ++nspace) {
            if (nspace->first == name_space) {
                return &(*nspace);
            }
        }

        return nullptr;
    }

    Model::namespace_t* Model::selectNamespace(const std::string& name_space)
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
