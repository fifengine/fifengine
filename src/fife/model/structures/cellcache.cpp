// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "cellcache.h"

// Standard C++ library includes
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "cell.h"
#include "instance.h"
#include "instancetree.h"
#include "layer.h"
#include "map.h"
#include "model/metamodel/grids/cellgrid.h"
#include "util/log/logger.h"
#include "util/structures/purge.h"

namespace FIFE
{

    static Logger _log(LM_STRUCTURES);

    class CellCacheChangeListener : public LayerChangeListener
    {
        public:
            explicit CellCacheChangeListener(Layer* layer) : m_layer(layer)
            {
            }
            ~CellCacheChangeListener() override = default;

            void onLayerChanged(Layer* layer, std::vector<Instance*>& instances) override
            {
                for (auto& instance : instances) {
                    if (instance->isMultiCell()) {
                        bool const rotchange = (instance->getChangeInfo() & ICHANGE_ROTATION) == ICHANGE_ROTATION;
                        bool const locchange = (instance->getChangeInfo() & ICHANGE_LOC) == ICHANGE_LOC;
                        bool const celchange = (instance->getChangeInfo() & ICHANGE_CELL) == ICHANGE_CELL;
                        bool const blochange = (instance->getChangeInfo() & ICHANGE_BLOCK) == ICHANGE_BLOCK;

                        if (!rotchange && !locchange && !celchange && !blochange) {
                            continue;
                        }

                        int32_t oldrotation       = instance->getOldRotation();
                        int32_t const newrotation = instance->getRotation();
                        if (!rotchange) {
                            oldrotation = newrotation;
                        }

                        if (rotchange || locchange || celchange) {
                            // update visual positions
                            instance->updateMultiInstances();
                        }
                        if (rotchange || celchange) {
                            // update cache positions
                            ModelCoordinate oldmc;
                            ModelCoordinate newmc;
                            if (m_layer == layer) {
                                oldmc = instance->getOldLocationRef().getLayerCoordinates();
                                newmc = instance->getLocationRef().getLayerCoordinates();
                            } else {
                                oldmc =
                                    m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                                        instance->getOldLocationRef().getExactLayerCoordinatesRef()));
                                newmc =
                                    m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                                        instance->getLocationRef().getExactLayerCoordinatesRef()));
                            }

                            if (!celchange) {
                                oldmc = newmc;
                            }

                            CellGrid* cg                                  = m_layer->getCellGrid();
                            std::vector<Instance*> const & multiinstances = instance->getMultiInstances();
                            auto it                                       = multiinstances.begin();
                            for (; it != multiinstances.end(); ++it) {
                                // remove
                                std::vector<ModelCoordinate> coordinates = cg->toMultiCoordinates(
                                    oldmc, (*it)->getObject()->getMultiPartCoordinates(oldrotation));
                                auto mcit = coordinates.begin();
                                for (; mcit != coordinates.end(); ++mcit) {
                                    Cell* cell = m_layer->getCellCache()->getCell(*mcit);
                                    if (cell != nullptr) {
                                        cell->removeInstance(*it);
                                    }
                                }
                                // add
                                coordinates = cg->toMultiCoordinates(
                                    newmc, (*it)->getObject()->getMultiPartCoordinates(newrotation));
                                mcit = coordinates.begin();
                                for (; mcit != coordinates.end(); ++mcit) {
                                    Cell* cell = m_layer->getCellCache()->getCell(*mcit);
                                    if (cell != nullptr) {
                                        cell->addInstance(*it);
                                    }
                                }
                            }

                            if (celchange) {
                                // leader instance
                                Cell* oldcell = m_layer->getCellCache()->getCell(oldmc);
                                Cell* newcell = m_layer->getCellCache()->getCell(newmc);
                                if (oldcell == newcell) {
                                    continue;
                                }
                                if (oldcell != nullptr) {
                                    oldcell->removeInstance(instance);
                                }
                                if (newcell != nullptr) {
                                    newcell->addInstance(instance);
                                }
                            }
                        }
                        continue;
                    }
                    if (instance->getObject()->isMultiPart()) {
                        continue;
                    }
                    if ((instance->getChangeInfo() & ICHANGE_BLOCK) == ICHANGE_BLOCK) {
                        ModelCoordinate mc;
                        if (m_layer == layer) {
                            mc = instance->getLocationRef().getLayerCoordinates();
                        } else {
                            mc = m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                                instance->getLocationRef().getExactLayerCoordinatesRef()));
                        }
                        Cell* cell = m_layer->getCellCache()->getCell(mc);
                        if (cell != nullptr) {
                            cell->changeInstance(instance);
                        }
                    }
                    if ((instance->getChangeInfo() & ICHANGE_CELL) == ICHANGE_CELL) {
                        ModelCoordinate oldmc;
                        ModelCoordinate newmc;
                        if (m_layer == layer) {
                            oldmc = instance->getOldLocationRef().getLayerCoordinates();
                            newmc = instance->getLocationRef().getLayerCoordinates();
                        } else {
                            oldmc = m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                                instance->getOldLocationRef().getExactLayerCoordinatesRef()));
                            newmc = m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                                instance->getLocationRef().getExactLayerCoordinatesRef()));
                        }

                        Cell* oldcell = m_layer->getCellCache()->getCell(oldmc);
                        Cell* newcell = m_layer->getCellCache()->getCell(newmc);
                        if (oldcell == newcell) {
                            continue;
                        }
                        if (oldcell != nullptr) {
                            oldcell->removeInstance(instance);
                        }
                        if (newcell != nullptr) {
                            newcell->addInstance(instance);
                        }
                    }
                }
            }

            void onInstanceCreate(Layer* layer, Instance* instance) override
            {
                ModelCoordinate mc;
                if (m_layer == layer) {
                    mc = instance->getLocationRef().getLayerCoordinates();
                } else {
                    mc = m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                        instance->getLocationRef().getExactLayerCoordinatesRef()));
                }

                CellCache* cache = m_layer->getCellCache();
                Location loc(m_layer);
                loc.setLayerCoordinates(mc);
                if (!cache->isInCellCache(loc)) {
                    cache->resize();
                }
                if (instance->isMultiCell()) {
                    instance->updateMultiInstances();
                    CellGrid* cg                                  = m_layer->getCellGrid();
                    std::vector<Instance*> const & multiinstances = instance->getMultiInstances();
                    auto it                                       = multiinstances.begin();
                    for (; it != multiinstances.end(); ++it) {
                        std::vector<ModelCoordinate> coordinates = cg->toMultiCoordinates(
                            mc, (*it)->getObject()->getMultiPartCoordinates(instance->getRotation()));
                        auto mcit = coordinates.begin();
                        for (; mcit != coordinates.end(); ++mcit) {
                            loc.setLayerCoordinates(*mcit);
                            if (!cache->isInCellCache(loc)) {
                                cache->resize();
                            }
                            Cell* cell = cache->getCell(*mcit);
                            if (cell != nullptr) {
                                cell->addInstance(*it);
                            }
                        }
                    }
                }
                Cell* cell = cache->getCell(mc);
                if (cell != nullptr) {
                    cell->addInstance(instance);
                }
            }

            void onInstanceDelete(Layer* layer, Instance* instance) override
            {
                ModelCoordinate mc;
                if (m_layer == layer) {
                    mc = instance->getLocationRef().getLayerCoordinates();
                } else {
                    mc = m_layer->getCellGrid()->toLayerCoordinates(layer->getCellGrid()->toMapCoordinates(
                        instance->getLocationRef().getExactLayerCoordinatesRef()));
                }

                CellCache* cache = m_layer->getCellCache();
                if (instance->isMultiCell()) {
                    instance->updateMultiInstances();
                    CellGrid* cg                                  = m_layer->getCellGrid();
                    std::vector<Instance*> const & multiinstances = instance->getMultiInstances();
                    auto it                                       = multiinstances.begin();
                    for (; it != multiinstances.end(); ++it) {
                        std::vector<ModelCoordinate> coordinates = cg->toMultiCoordinates(
                            mc, (*it)->getObject()->getMultiPartCoordinates(instance->getRotation()));
                        auto mcit = coordinates.begin();
                        for (; mcit != coordinates.end(); ++mcit) {
                            Cell* cell = cache->getCell(*mcit);
                            if (cell != nullptr) {
                                cell->removeInstance(*it);
                            }
                        }
                    }
                }
                Cell* cell = cache->getCell(mc);
                if (cell != nullptr) {
                    cell->removeInstance(instance);
                }
                // updates size on the next cache update (happens on same pump)
                cache->setSizeUpdate(true);
            }

        private:
            Layer* m_layer;
    };

    Zone::Zone(uint32_t id) : m_id(id)
    {
    }

    Zone::~Zone()
    {
        for (auto* m_cell : m_cells) {
            m_cell->resetZone();
        }
    }

    void Zone::addCell(Cell* cell)
    {
        if (cell->getZone() == nullptr) {
            cell->setZone(this);
            m_cells.insert(cell);
        }
    }

    void Zone::removeCell(Cell* cell)
    {
        auto i = m_cells.find(cell);
        if (i != m_cells.end()) {
            (*i)->resetZone();
            m_cells.erase(i);
        }
    }

    void Zone::mergeZone(Zone* zone)
    {
        std::set<Cell*> const & cells = zone->getCells();
        m_cells.insert(cells.begin(), cells.end());
        for (auto* cell : cells) {
            cell->setZone(this);
        }
        zone->resetCells();
    }

    std::set<Cell*> const & Zone::getCells() const
    {
        return m_cells;
    }

    void Zone::resetCells()
    {
        m_cells.clear();
    }

    uint32_t Zone::getId() const
    {
        return m_id;
    }

    uint32_t Zone::getCellCount() const
    {
        return static_cast<uint32_t>(m_cells.size());
    }

    std::vector<Cell*> Zone::getTransitionCells(Layer const * layer)
    {
        std::vector<Cell*> transitions;
        auto it = m_cells.begin();
        for (; it != m_cells.end(); ++it) {
            TransitionInfo const * trans = (*it)->getTransition();
            if (trans == nullptr) {
                continue;
            }
            if (layer != nullptr) {
                if (layer == (*it)->getLayer()) {
                    transitions.push_back(*it);
                }
            } else {
                transitions.push_back(*it);
            }
        }
        return transitions;
    }

    class ZoneCellChangeListener : public CellChangeListener
    {
        public:
            explicit ZoneCellChangeListener(CellCache* cache) : m_cache(cache)
            {
            }
            ~ZoneCellChangeListener() override = default;

            void onInstanceEnteredCell([[maybe_unused]] Cell* cell, [[maybe_unused]] Instance* instance) override
            {
            }

            void onInstanceExitedCell([[maybe_unused]] Cell* cell, [[maybe_unused]] Instance* instance) override
            {
            }

            void onBlockingChangedCell(Cell* cell, [[maybe_unused]] CellTypeInfo type, bool blocks) override
            {
                if (blocks) {
                    cell->setZoneProtected(true);
                    m_cache->splitZone(cell);
                } else {
                    Zone* z1                             = cell->getZone();
                    Zone* z2                             = nullptr;
                    std::vector<Cell*> const & neighbors = cell->getNeighbors();
                    auto it                              = neighbors.begin();
                    for (; it != neighbors.end(); ++it) {
                        Zone* z = (*it)->getZone();
                        if ((z != nullptr) && z != z1) {
                            z2 = z;
                        }
                    }
                    if ((z1 != nullptr) && (z2 != nullptr)) {
                        cell->setZoneProtected(false);
                        m_cache->mergeZones(z1, z2);
                    }
                }
            }

        private:
            CellCache* m_cache;
    };

    CellCache::CellCache(Layer* layer) :
        m_layer(layer),
        m_defaultCostMulti(1.0),
        m_defaultSpeedMulti(1.0),
        m_cellListener(new CellCacheChangeListener(m_layer)),
        m_neighborZ(-1),
        m_blockingUpdate(false),
        m_sizeUpdate(false),
        m_searchNarrow(true),
        m_staticSize(false),
        m_cellZoneListener(nullptr)
    {
        m_cellZoneListener = new ZoneCellChangeListener(this);
        // set base size
        ModelCoordinate min;
        ModelCoordinate max;
        m_layer->getMinMaxCoordinates(min, max);
        m_size.w = max.x;
        m_size.h = max.y;
        m_size.x = min.x;
        m_size.y = min.y;

        m_layer->addChangeListener(m_cellListener);
        m_layer->addChangeListener(m_cellListener);
        std::vector<Layer*> const & interacts = m_layer->getInteractLayers();
        if (!interacts.empty()) {
            auto layit = interacts.begin();
            for (; layit != interacts.end(); ++layit) {
                // set size
                (*layit)->getMinMaxCoordinates(min, max, m_layer);
                m_size.w = std::max(max.x, m_size.w);
                m_size.h = std::max(max.y, m_size.h);
                m_size.x = std::min(min.x, m_size.x);
                m_size.y = std::min(min.y, m_size.y);
                // add listener
                (*layit)->addChangeListener(m_cellListener);
            }
        }

        m_width  = std::abs(m_size.w - m_size.x) + 1;
        m_height = std::abs(m_size.h - m_size.y) + 1;

        m_cells.resize(m_width);
        for (uint32_t i = 0; i < m_width; ++i) {
            m_cells[i].resize(m_height, nullptr);
        }
    }

    CellCache::~CellCache()
    {
        // reset cache
        reset();
        // remove listener from layers
        m_layer->removeChangeListener(m_cellListener);
        std::vector<Layer*> const & interacts = m_layer->getInteractLayers();
        if (!interacts.empty()) {
            auto layit = interacts.begin();
            for (; layit != interacts.end(); ++layit) {
                (*layit)->removeChangeListener(m_cellListener);
            }
        }
        // delete listener
        delete m_cellListener;
        delete m_cellZoneListener;
    }

    void CellCache::reset()
    {
        // delete zones
        if (!m_zones.empty()) {
            auto it = m_zones.begin();
            for (; it != m_zones.end(); ++it) {
                delete *it;
            }
            m_zones.clear();
        }
        // clear all containers
        m_costsToCells.clear();
        m_costsTable.clear();
        m_costMultipliers.clear();
        m_speedMultipliers.clear();
        m_narrowCells.clear();
        m_cellAreas.clear();
        // delete cells
        if (!m_cells.empty()) {
            auto it = m_cells.begin();
            for (; it != m_cells.end(); ++it) {
                auto cit = (*it).begin();
                for (; cit != (*it).end(); ++cit) {
                    delete *cit;
                }
            }
            m_cells.clear();
        }
        // reset default cost and speed
        m_defaultCostMulti  = 1.0;
        m_defaultSpeedMulti = 1.0;
        // reset size
        m_size.x = 0;
        m_size.y = 0;
        m_size.w = 0;
        m_size.h = 0;
        m_width  = 0;
        m_height = 0;
    }

    void CellCache::resize()
    {
        if (m_staticSize) {
            return;
        }
        // get new size and check if it has changed
        Rect const newsize = calculateCurrentSize();
        resize(newsize);
    }

    void CellCache::resize(Rect const & rec)
    {
        // check if size has changed
        Rect const newsize = rec;
        if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
            uint32_t const w = std::abs(newsize.w - newsize.x) + 1;
            uint32_t const h = std::abs(newsize.h - newsize.y) + 1;

            std::vector<std::vector<Cell*>> cells;
            cells.resize(w);
            for (uint32_t i = 0; i < w; ++i) {
                cells[i].resize(h, nullptr);
            }
            std::vector<Layer*> const & interacts = m_layer->getInteractLayers();
            for (uint32_t y = 0; y < h; ++y) {
                for (uint32_t x = 0; x < w; ++x) {
                    // transfer cells
                    ModelCoordinate const mc(newsize.x + x, newsize.y + y);
                    Cell* cell          = nullptr;
                    int32_t const old_x = mc.x - m_size.x;
                    int32_t const old_y = mc.y - m_size.y;
                    // out of range in the old size, so we create a new cell
                    if (old_x < 0 || std::cmp_greater_equal(old_x, m_width) || old_y < 0 ||
                        std::cmp_greater_equal(old_y, m_height)) {
                        int32_t const coordId = x + (y * w);
                        cell                  = new Cell(coordId, mc, m_layer);
                        cells[x][y]           = cell;

                        std::list<Instance*> cell_instances;
                        m_layer->getInstanceTree()->findInstances(mc, 0, 0, cell_instances);
                        if (!interacts.empty()) {
                            // fill interact Instances into Cell
                            auto it = interacts.begin();
                            std::list<Instance*> interact_instances;
                            for (; it != interacts.end(); ++it) {
                                // convert coordinates
                                ExactModelCoordinate const emc(FIFE::intPt2doublePt(mc));
                                ModelCoordinate const inter_mc = (*it)->getCellGrid()->toLayerCoordinates(
                                    m_layer->getCellGrid()->toMapCoordinates(emc));
                                // check interact layer for instances
                                (*it)->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
                                if (!interact_instances.empty()) {
                                    cell_instances.insert(
                                        cell_instances.end(), interact_instances.begin(), interact_instances.end());
                                    interact_instances.clear();
                                }
                            }
                        }
                        if (!cell_instances.empty()) {
                            // add instances to cell
                            cell->addInstances(cell_instances);
                        }
                        // transfer ownership
                    } else {
                        cell = m_cells[static_cast<uint32_t>(old_x)][static_cast<uint32_t>(old_y)];
                        m_cells[static_cast<uint32_t>(old_x)][static_cast<uint32_t>(old_y)] = nullptr;
                        cells[x][y]                                                         = cell;
                        int32_t const coordId                                               = x + (y * w);
                        cell->setCellId(coordId);
                        cell->resetNeighbors();
                    }
                }
            }
            // delete old unused cells
            auto it = m_cells.begin();
            for (; it != m_cells.end(); ++it) {
                auto cit = (*it).begin();
                for (; cit != (*it).end(); ++cit) {
                    if (*cit != nullptr) {
                        delete *cit;
                        *cit = nullptr;
                    }
                }
            }
            // use new values
            m_cells  = cells;
            m_size   = newsize;
            m_width  = w;
            m_height = h;

            bool const zCheck = m_neighborZ != -1;
            // fill neighbors into cells
            it = m_cells.begin();
            for (; it != m_cells.end(); ++it) {
                auto cit = (*it).begin();
                for (; cit != (*it).end(); ++cit) {
                    int32_t const cellZ = (*cit)->getLayerCoordinates().z;
                    std::vector<ModelCoordinate> coordinates;
                    m_layer->getCellGrid()->getAccessibleCoordinates((*cit)->getLayerCoordinates(), coordinates);
                    for (auto& coordinate : coordinates) {
                        Cell* c = getCell(coordinate);
                        if (*cit == c || (c == nullptr)) {
                            continue;
                        }
                        if (zCheck) {
                            if (std::abs(c->getLayerCoordinates().z - cellZ) > m_neighborZ) {
                                continue;
                            }
                        }
                        (*cit)->addNeighbor(c);
                    }
                }
            }
        }
    }

    void CellCache::createCells()
    {
        std::vector<Layer*> const & interacts = m_layer->getInteractLayers();
        for (uint32_t y = 0; y < m_height; ++y) {
            for (uint32_t x = 0; x < m_width; ++x) {
                ModelCoordinate const mc(m_size.x + x, m_size.y + y);
                Cell* cell = getCell(mc);
                if (cell == nullptr) {
                    cell          = new Cell(convertCoordToInt(mc), mc, m_layer);
                    m_cells[x][y] = cell;
                }
                // fill Instances into Cell
                std::list<Instance*> cell_instances;
                m_layer->getInstanceTree()->findInstances(mc, 0, 0, cell_instances);
                if (!interacts.empty()) {
                    // fill interact Instances into Cell
                    auto it = interacts.begin();
                    std::list<Instance*> interact_instances;
                    for (; it != interacts.end(); ++it) {
                        // convert coordinates
                        ExactModelCoordinate const emc(FIFE::intPt2doublePt(mc));
                        ModelCoordinate const inter_mc =
                            (*it)->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
                        // check interact layer for instances
                        (*it)->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
                        if (!interact_instances.empty()) {
                            cell_instances.insert(
                                cell_instances.end(), interact_instances.begin(), interact_instances.end());
                            interact_instances.clear();
                        }
                    }
                }
                if (!cell_instances.empty()) {
                    // add instances to cell
                    cell->addInstances(cell_instances);
                }
            }
        }
        // fill neighbors into cells
        auto it = m_cells.begin();
        for (; it != m_cells.end(); ++it) {
            auto cit = (*it).begin();
            for (; cit != (*it).end(); ++cit) {
                uint8_t accessible = 0;
                bool const selfblocker =
                    (*cit)->getCellType() == CTYPE_STATIC_BLOCKER || (*cit)->getCellType() == CTYPE_CELL_BLOCKER;
                std::vector<ModelCoordinate> coordinates;
                m_layer->getCellGrid()->getAccessibleCoordinates((*cit)->getLayerCoordinates(), coordinates);
                for (auto& coordinate : coordinates) {
                    Cell* c = getCell(coordinate);
                    if (*cit == c || (c == nullptr)) {
                        continue;
                    }
                    if (!selfblocker && c->getCellType() != CTYPE_STATIC_BLOCKER &&
                        c->getCellType() != CTYPE_CELL_BLOCKER) {
                        ++accessible;
                    }
                    (*cit)->addNeighbor(c);
                }
                // add cell to narrow cells and add listener for zone change
                if (m_searchNarrow && !selfblocker && accessible < 3) {
                    addNarrowCell(*cit);
                }
            }
        }
        // create Zones
        it = m_cells.begin();
        for (; it != m_cells.end(); ++it) {
            auto cit = (*it).begin();
            for (; cit != (*it).end(); ++cit) {
                Cell* cell = *cit;
                if ((cell->getZone() != nullptr) || cell->isInserted()) {
                    continue;
                }
                if (cell->getCellType() == CTYPE_STATIC_BLOCKER || cell->getCellType() == CTYPE_CELL_BLOCKER) {
                    continue;
                }
                Zone* zone = createZone();
                cell->setInserted(true);
                std::stack<Cell*> cellstack;
                cellstack.push(cell);
                while (!cellstack.empty()) {
                    Cell* c = cellstack.top();
                    cellstack.pop();
                    zone->addCell(c);

                    std::vector<Cell*> const & neighbors = c->getNeighbors();
                    for (auto* nc : neighbors) {
                        if (!nc->isInserted() && nc->getCellType() != CTYPE_STATIC_BLOCKER &&
                            nc->getCellType() != CTYPE_CELL_BLOCKER) {
                            nc->setInserted(true);
                            cellstack.push(nc);
                        }
                    }
                }
            }
        }
    }

    void CellCache::forceUpdate()
    {
        auto it = m_cells.begin();
        for (; it != m_cells.end(); ++it) {
            auto cit = (*it).begin();
            for (; cit != (*it).end(); ++cit) {
                (*cit)->updateCellInfo();
            }
        }
    }

    void CellCache::addCell(Cell* cell)
    {
        ModelCoordinate const mc                      = cell->getLayerCoordinates();
        m_cells[(mc.x - m_size.x)][(mc.y - m_size.y)] = cell;
    }

    Cell* CellCache::createCell(ModelCoordinate const & mc)
    {
        Cell* cell = getCell(mc);
        if (cell == nullptr) {
            cell                                          = new Cell(convertCoordToInt(mc), mc, m_layer);
            m_cells[(mc.x - m_size.x)][(mc.y - m_size.y)] = cell;
        }
        return cell;
    }

    Cell* CellCache::getCell(ModelCoordinate const & mc)
    {
        int32_t const x = mc.x - m_size.x;
        int32_t const y = mc.y - m_size.y;

        if (x < 0 || std::cmp_greater_equal(x, m_width) || y < 0 || std::cmp_greater_equal(y, m_height)) {
            return nullptr;
        }

        return m_cells[static_cast<uint32_t>(x)][static_cast<uint32_t>(y)];
    }

    std::vector<std::vector<Cell*>> const & CellCache::getCells()
    {
        return m_cells;
    }

    void CellCache::removeCell(Cell* cell)
    {
        if (!m_costsToCells.empty()) {
            removeCellFromCost(cell);
        }
        if (!m_costMultipliers.empty()) {
            resetCostMultiplier(cell);
        }
        if (!m_speedMultipliers.empty()) {
            resetSpeedMultiplier(cell);
        }
        if (!m_narrowCells.empty()) {
            removeNarrowCell(cell);
        }
        if (!m_cellAreas.empty()) {
            removeCellFromArea(cell);
        }
    }

    void CellCache::addInteractOnRuntime(Layer* interact)
    {
        interact->setInteract(true, m_layer->getId());
        m_layer->addInteractLayer(interact);
        interact->addChangeListener(m_cellListener);
        Rect const newsize = calculateCurrentSize();
        if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
            resize();
        }
        // not optimal but needed if the grids have different geometry
        for (uint32_t y = 0; y < m_height; ++y) {
            for (uint32_t x = 0; x < m_width; ++x) {
                ModelCoordinate const mc(m_size.x + x, m_size.y + y);
                Cell* cell = getCell(mc);
                if (cell != nullptr) {
                    // convert coordinates
                    ExactModelCoordinate const emc(FIFE::intPt2doublePt(mc));
                    ModelCoordinate const inter_mc =
                        interact->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
                    // check interact layer for instances
                    std::list<Instance*> interact_instances;
                    interact->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
                    if (!interact_instances.empty()) {
                        // fill interact Instances into Cell
                        cell->addInstances(interact_instances);
                    }
                }
            }
        }
    }

    void CellCache::removeInteractOnRuntime(Layer* interact)
    {
        interact->setInteract(false, "");
        m_layer->removeInteractLayer(interact);
        Rect const newsize = calculateCurrentSize();
        if (newsize.x != m_size.x || newsize.y != m_size.y || newsize.w != m_size.w || newsize.h != m_size.h) {
            resize();
        }
        // not optimal but needed if the grids have different geometry
        for (uint32_t y = 0; y < m_height; ++y) {
            for (uint32_t x = 0; x < m_width; ++x) {
                ModelCoordinate const mc(m_size.x + x, m_size.y + y);
                Cell* cell = getCell(mc);
                if (cell != nullptr) {
                    // convert coordinates
                    ExactModelCoordinate const emc(FIFE::intPt2doublePt(mc));
                    ModelCoordinate const inter_mc =
                        interact->getCellGrid()->toLayerCoordinates(m_layer->getCellGrid()->toMapCoordinates(emc));
                    // check interact layer for instances
                    std::list<Instance*> interact_instances;
                    interact->getInstanceTree()->findInstances(inter_mc, 0, 0, interact_instances);
                    if (!interact_instances.empty()) {
                        // remove interact Instances from Cell
                        for (auto& interact_instance : interact_instances) {
                            cell->removeInstance(interact_instance);
                        }
                    }
                }
            }
        }
    }

    LayerChangeListener* CellCache::getCellCacheChangeListener()
    {
        return m_cellListener;
    }

    Layer* CellCache::getLayer()
    {
        return m_layer;
    }

    Rect const & CellCache::getSize()
    {
        return m_size;
    }

    void CellCache::setSize(Rect const & rec)
    {
        resize(rec);
    }

    uint32_t CellCache::getWidth() const
    {
        return m_width;
    }

    uint32_t CellCache::getHeight() const
    {
        return m_height;
    }

    bool CellCache::isInCellCache(Location const & location) const
    {
        if (m_layer != location.getLayer()) {
            return false;
        }
        int32_t const x = location.getLayerCoordinates().x - m_size.x;
        int32_t const y = location.getLayerCoordinates().y - m_size.y;

        return x >= 0 && std::cmp_less(x, m_width) && y >= 0 && std::cmp_less(y, m_height);
    }

    int32_t CellCache::convertCoordToInt(ModelCoordinate const & coord) const
    {
        ModelCoordinate const newcoords(coord.x - m_size.x, coord.y - m_size.y);
        return newcoords.x + (newcoords.y * m_width);
    }

    ModelCoordinate CellCache::convertIntToCoord(int32_t const cell) const
    {
        ModelCoordinate const coord((cell % m_width) + m_size.x, (cell / m_width) + m_size.y);
        return coord;
    }

    int32_t CellCache::getMaxIndex() const
    {
        int32_t const max_index = m_width * m_height;
        return max_index;
    }

    void CellCache::setMaxNeighborZ(int32_t z)
    {
        m_neighborZ = z;
    }

    int32_t CellCache::getMaxNeighborZ() const
    {
        return m_neighborZ;
    }

    std::vector<Cell*> CellCache::getCellsInLine(ModelCoordinate const & pt1, ModelCoordinate const & pt2, bool blocker)
    {
        std::vector<Cell*> cells;
        std::vector<ModelCoordinate> const coords = m_layer->getCellGrid()->getCoordinatesInLine(pt1, pt2);
        for (auto const & coord : coords) {
            Cell* c = getCell(coord);
            if (c != nullptr) {
                if (blocker && c->getCellType() != CTYPE_NO_BLOCKER) {
                    return cells;
                }
                cells.push_back(c);
            } else {
                return cells;
            }
        }
        return cells;
    }

    std::vector<Cell*> CellCache::getCellsInRect(Rect const & rec)
    {
        std::vector<Cell*> cells;
        cells.reserve(static_cast<std::size_t>(rec.w) * static_cast<std::size_t>(rec.h));

        ModelCoordinate current(rec.x, rec.y);
        ModelCoordinate const target(rec.x + rec.w, rec.y + rec.h);
        for (; current.y < target.y; ++current.y) {
            current.x = rec.x;
            for (; current.x < target.x; ++current.x) {
                Cell* c = getCell(current);
                if (c != nullptr) {
                    cells.push_back(c);
                }
            }
        }
        return cells;
    }

    std::vector<Cell*> CellCache::getBlockingCellsInRect(Rect const & rec)
    {
        std::vector<Cell*> cells;
        cells.reserve(static_cast<std::size_t>(rec.w) * static_cast<std::size_t>(rec.h));

        ModelCoordinate current(rec.x, rec.y);
        ModelCoordinate const target(rec.x + rec.w, rec.y + rec.h);
        for (; current.y < target.y; ++current.y) {
            current.x = rec.x;
            for (; current.x < target.x; ++current.x) {
                Cell* c = getCell(current);
                if ((c != nullptr) && c->getCellType() != CTYPE_NO_BLOCKER) {
                    cells.push_back(c);
                }
            }
        }
        return cells;
    }

    std::vector<Cell*> CellCache::getCellsInCircle(ModelCoordinate const & center, uint16_t radius)
    {
        std::vector<Cell*> cells;
        // radius power 2
        uint16_t const radiusp2 = (radius + 1) * radius;

        ModelCoordinate current(center.x - radius, center.y - radius);
        ModelCoordinate const target(center.x + radius, center.y + radius);
        for (; current.y < center.y; current.y++) {
            current.x = center.x - radius;
            for (; current.x < center.x; current.x++) {
                Cell* c = getCell(current);
                if (c != nullptr) {
                    uint16_t const dx       = center.x - current.x;
                    uint16_t const dy       = center.y - current.y;
                    uint16_t const distance = (dx * dx) + (dy * dy);
                    if (distance <= radiusp2) {
                        cells.push_back(c);

                        current.x = center.x + dx;
                        c         = getCell(current);
                        if (c != nullptr) {
                            cells.push_back(c);
                        }

                        current.y = center.y + dy;
                        c         = getCell(current);
                        if (c != nullptr) {
                            cells.push_back(c);
                        }

                        current.x = center.x - dx;
                        c         = getCell(current);
                        if (c != nullptr) {
                            cells.push_back(c);
                        }

                        current.y = center.y - dy;
                    }
                }
            }
        }
        current.x = center.x;
        current.y = center.y - radius;
        for (; current.y <= target.y; current.y++) {
            Cell* c = getCell(current);
            if (c != nullptr) {
                cells.push_back(c);
            }
        }

        current.y = center.y;
        current.x = center.x - radius;
        for (; current.x <= target.x; current.x++) {
            Cell* c = getCell(current);
            if (c != nullptr) {
                cells.push_back(c);
            }
        }
        return cells;
    }

    std::vector<Cell*> CellCache::getCellsInCircleSegment(
        ModelCoordinate const & center, uint16_t radius, int32_t sangle, int32_t eangle)
    {
        std::vector<Cell*> cells;
        ExactModelCoordinate const exactCenter(center.x, center.y);
        std::vector<Cell*> const tmpCells = getCellsInCircle(center, radius);
        int32_t const s                   = (sangle + 360) % 360;
        int32_t const e                   = (eangle + 360) % 360;
        bool const greater                = s > e;
        for (auto const & tmpCell : tmpCells) {
            int32_t const angle = getAngleBetween(exactCenter, intPt2doublePt(tmpCell->getLayerCoordinates()));
            if (greater) {
                if (angle >= s || angle <= e) {
                    cells.push_back(tmpCell);
                }
            } else {
                if (angle >= s && angle <= e) {
                    cells.push_back(tmpCell);
                }
            }
        }
        return cells;
    }

    void CellCache::registerCost(std::string const & costId, double cost)
    {
        std::pair<std::map<std::string, double>::iterator, bool> insertiter;
        insertiter = m_costsTable.insert(std::pair<std::string, double>(costId, cost));
        if (!insertiter.second) {
            double& old_cost = insertiter.first->second;
            old_cost         = cost;
        }
    }

    void CellCache::unregisterCost(std::string const & costId)
    {
        auto it = m_costsTable.find(costId);
        if (it != m_costsTable.end()) {
            m_costsTable.erase(it);
            m_costsToCells.erase(costId);
        }
    }

    double CellCache::getCost(std::string const & costId)
    {
        auto it = m_costsTable.find(costId);
        if (it != m_costsTable.end()) {
            return it->second;
        }
        return 0.0;
    }

    bool CellCache::existsCost(std::string const & costId)
    {
        auto it = m_costsTable.find(costId);
        return it != m_costsTable.end();
    }

    std::list<std::string> CellCache::getCosts()
    {
        std::list<std::string> costs;
        auto it = m_costsTable.begin();
        for (; it != m_costsTable.end(); ++it) {
            costs.push_back((*it).first);
        }
        return costs;
    }

    void CellCache::unregisterAllCosts()
    {
        m_costsTable.clear();
        m_costsToCells.clear();
    }

    void CellCache::addCellToCost(std::string const & costId, Cell* cell)
    {
        if (existsCost(costId)) {
            StringCellPair const result = m_costsToCells.equal_range(costId);
            auto it                     = result.first;
            for (; it != result.second; ++it) {
                if ((*it).second == cell) {
                    return;
                }
            }
            m_costsToCells.insert(std::pair<std::string, Cell*>(costId, cell));
        }
    }

    void CellCache::addCellsToCost(std::string const & costId, std::vector<Cell*> const & cells)
    {
        auto it = cells.begin();
        for (; it != cells.end(); ++it) {
            addCellToCost(costId, *it);
        }
    }

    void CellCache::removeCellFromCost(Cell* cell)
    {
        auto it = m_costsToCells.begin();
        for (; it != m_costsToCells.end();) {
            if ((*it).second == cell) {
                m_costsToCells.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void CellCache::removeCellFromCost(std::string const & costId, Cell* cell)
    {
        StringCellPair const result = m_costsToCells.equal_range(costId);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            if ((*it).second == cell) {
                m_costsToCells.erase(it);
                break;
            }
        }
    }

    void CellCache::removeCellsFromCost(std::string const & costId, std::vector<Cell*> const & cells)
    {
        auto it = cells.begin();
        for (; it != cells.end(); ++it) {
            removeCellFromCost(costId, *it);
        }
    }

    std::vector<Cell*> CellCache::getCostCells(std::string const & costId)
    {
        std::vector<Cell*> cells;
        StringCellPair const result = m_costsToCells.equal_range(costId);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            cells.push_back((*it).second);
        }
        return cells;
    }

    std::vector<std::string> CellCache::getCellCosts(Cell* cell)
    {
        std::vector<std::string> costs;
        auto it = m_costsToCells.begin();
        for (; it != m_costsToCells.end(); ++it) {
            if ((*it).second == cell) {
                costs.push_back((*it).first);
            }
        }
        return costs;
    }

    bool CellCache::existsCostForCell(std::string const & costId, Cell* cell)
    {
        StringCellPair const result = m_costsToCells.equal_range(costId);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            if ((*it).second == cell) {
                return true;
            }
        }
        return false;
    }

    double CellCache::getAdjacentCost(ModelCoordinate const & adjacent, ModelCoordinate const & next)
    {
        double cost    = m_layer->getCellGrid()->getAdjacentCost(adjacent, next);
        Cell* nextcell = getCell(next);
        if (nextcell != nullptr) {
            if (!nextcell->defaultCost()) {
                cost *= nextcell->getCostMultiplier();
            } else {
                cost *= m_defaultCostMulti;
            }
        }
        return cost;
    }

    double CellCache::getAdjacentCost(
        ModelCoordinate const & adjacent, ModelCoordinate const & next, std::string const & costId)
    {
        double cost    = m_layer->getCellGrid()->getAdjacentCost(adjacent, next);
        Cell* nextcell = getCell(next);
        if (nextcell != nullptr) {
            if (existsCostForCell(costId, nextcell)) {
                cost *= getCost(costId);
            } else {
                if (!nextcell->defaultCost()) {
                    cost *= nextcell->getCostMultiplier();
                } else {
                    cost *= m_defaultCostMulti;
                }
            }
        }
        return cost;
    }

    bool CellCache::getCellSpeedMultiplier(ModelCoordinate const & cell, double& multiplier)
    {
        Cell* nextcell = getCell(cell);
        if (nextcell != nullptr) {
            if (!nextcell->defaultSpeed()) {
                multiplier = nextcell->getSpeedMultiplier();
                return true;
            }
        }
        multiplier = m_defaultSpeedMulti;
        return false;
    }

    void CellCache::setDefaultCostMultiplier(double multi)
    {
        m_defaultCostMulti = multi;
    }

    double CellCache::getDefaultCostMultiplier() const
    {
        return m_defaultCostMulti;
    }

    void CellCache::setDefaultSpeedMultiplier(double multi)
    {
        m_defaultSpeedMulti = multi;
    }

    double CellCache::getDefaultSpeedMultiplier() const
    {
        return m_defaultSpeedMulti;
    }

    bool CellCache::isDefaultCost(Cell* cell)
    {
        auto it = m_costMultipliers.find(cell);
        return it == m_costMultipliers.end();
    }

    void CellCache::setCostMultiplier(Cell* cell, double multi)
    {
        std::pair<std::map<Cell*, double>::iterator, bool> const insertiter =
            m_costMultipliers.insert(std::pair<Cell*, double>(cell, multi));
        if (!insertiter.second) {
            double& old = insertiter.first->second;
            old         = multi;
        }
    }

    double CellCache::getCostMultiplier(Cell* cell)
    {
        double cost = 1.0;
        auto it     = m_costMultipliers.find(cell);
        if (it != m_costMultipliers.end()) {
            cost = it->second;
        }
        return cost;
    }

    void CellCache::resetCostMultiplier(Cell* cell)
    {
        m_costMultipliers.erase(cell);
    }

    bool CellCache::isDefaultSpeed(Cell* cell)
    {
        auto it = m_speedMultipliers.find(cell);
        return it == m_speedMultipliers.end();
    }

    void CellCache::setSpeedMultiplier(Cell* cell, double multi)
    {
        std::pair<std::map<Cell*, double>::iterator, bool> const insertiter =
            m_speedMultipliers.insert(std::pair<Cell*, double>(cell, multi));
        if (!insertiter.second) {
            double& old = insertiter.first->second;
            old         = multi;
        }
    }

    double CellCache::getSpeedMultiplier(Cell* cell)
    {
        double speed = 1.0;
        auto it      = m_speedMultipliers.find(cell);
        if (it != m_speedMultipliers.end()) {
            speed = it->second;
        }
        return speed;
    }

    void CellCache::resetSpeedMultiplier(Cell* cell)
    {
        m_speedMultipliers.erase(cell);
    }

    void CellCache::addTransition(Cell* cell)
    {
        m_transitions.push_back(cell);
    }

    void CellCache::removeTransition(Cell* cell)
    {
        auto it = m_transitions.begin();
        for (; it != m_transitions.end(); ++it) {
            if (cell == *it) {
                m_transitions.erase(it);
                break;
            }
        }
    }

    std::vector<Cell*> CellCache::getTransitionCells(Layer* layer)
    {
        if (layer == nullptr) {
            return m_transitions;
        }
        std::vector<Cell*> cells;
        auto it = m_transitions.begin();
        for (; it != m_transitions.end(); ++it) {
            TransitionInfo const * trans = (*it)->getTransition();
            if (trans != nullptr) {
                if (trans->m_layer == layer) {
                    cells.push_back(*it);
                }
            }
        }
        return cells;
    }

    Zone* CellCache::createZone()
    {
        uint32_t id = 0;
        bool search = true;
        while (search) {
            bool found = false;
            if (!m_zones.empty()) {
                if (std::ranges::any_of(m_zones, [id](Zone const * z) {
                        return z->getId() == id;
                    })) {
                    found = true;
                    ++id;
                }
            }
            search = found;
        }
        Zone* zi = new Zone(id);
        m_zones.push_back(zi);

        return zi;
    }

    std::vector<Zone*> const & CellCache::getZones()
    {
        return m_zones;
    }

    Zone* CellCache::getZone(uint32_t id)
    {
        Zone* zi = nullptr;
        auto it  = std::ranges::find_if(m_zones, [id](Zone const * z) {
            return z->getId() == id;
        });
        if (it != m_zones.end()) {
            zi = *it;
        }

        if (zi == nullptr) {
            zi = new Zone(id);
            m_zones.push_back(zi);
        }

        return zi;
    }

    void CellCache::removeZone(Zone* zone)
    {
        auto it = std::ranges::find_if(m_zones, [zone](Zone const * z) {
            return z == zone;
        });
        if (it != m_zones.end()) {
            delete *it;
            m_zones.erase(it);
        }
    }

    void CellCache::splitZone(Cell* cell)
    {
        Zone* currentZone = cell->getZone();
        if (currentZone == nullptr) {
            return;
        }

        Zone* newZone = createZone();
        std::stack<Cell*> cellstack;
        std::vector<Cell*> const & neighbors = cell->getNeighbors();
        auto it                              = std::ranges::find_if(neighbors, [](Cell* nc) {
            return nc->isInserted() && !nc->isZoneProtected() && nc->getCellType() != CTYPE_STATIC_BLOCKER &&
                   nc->getCellType() != CTYPE_CELL_BLOCKER;
        });
        if (it != neighbors.end()) {
            cellstack.push(*it);
        }

        while (!cellstack.empty()) {
            Cell* c = cellstack.top();
            cellstack.pop();

            currentZone->removeCell(c);
            newZone->addCell(c);
            c->setInserted(true);
            if (c->isZoneProtected()) {
                continue;
            }
            std::vector<Cell*> const & neigh = c->getNeighbors();
            for (auto* nc : neigh) {
                if (nc->getZone() == currentZone && nc->isInserted() && nc->getCellType() != CTYPE_STATIC_BLOCKER &&
                    nc->getCellType() != CTYPE_CELL_BLOCKER) {
                    cellstack.push(nc);
                    nc->setInserted(false);
                }
            }
        }
        if (currentZone->getCellCount() == 0) {
            removeZone(currentZone);
        }
    }

    void CellCache::mergeZones(Zone* zone1, Zone* zone2)
    {
        if ((zone1 == nullptr) || (zone2 == nullptr)) {
            return;
        }
        Zone* addZone = zone2;
        Zone* oldZone = zone1;
        if (zone1->getCellCount() > zone2->getCellCount()) {
            addZone = zone1;
            oldZone = zone2;
        }
        addZone->mergeZone(oldZone);
        removeZone(oldZone);
    }

    void CellCache::addNarrowCell(Cell* cell)
    {
        std::pair<std::set<Cell*>::iterator, bool> const insertiter = m_narrowCells.insert(cell);
        if (insertiter.second) {
            cell->addChangeListener(m_cellZoneListener);
        }
    }

    std::set<Cell*> const & CellCache::getNarrowCells()
    {
        return m_narrowCells;
    }

    void CellCache::removeNarrowCell(Cell* cell)
    {
        auto it = m_narrowCells.find(cell);
        if (it != m_narrowCells.end()) {
            (*it)->removeChangeListener(m_cellZoneListener);
            m_narrowCells.erase(it);
        }
    }

    void CellCache::resetNarrowCells()
    {
        auto it = m_narrowCells.begin();
        for (; it != m_narrowCells.end(); ++it) {
            (*it)->removeChangeListener(m_cellZoneListener);
        }
        m_narrowCells.clear();
    }

    bool CellCache::isSearchNarrowCells() const
    {
        return m_searchNarrow;
    }

    void CellCache::setSearchNarrowCells(bool search)
    {
        m_searchNarrow = search;
    }

    void CellCache::addCellToArea(std::string const & id, Cell* cell)
    {
        m_cellAreas.insert(std::pair<std::string, Cell*>(id, cell));
    }

    void CellCache::addCellsToArea(std::string const & id, std::vector<Cell*> const & cells)
    {
        auto it = cells.begin();
        for (; it != cells.end(); ++it) {
            addCellToArea(id, *it);
        }
    }

    void CellCache::removeCellFromArea(Cell* cell)
    {
        auto it = m_cellAreas.begin();
        while (it != m_cellAreas.end()) {
            if ((*it).second == cell) {
                m_cellAreas.erase(it++);
            } else {
                ++it;
            }
        }
    }

    void CellCache::removeCellFromArea(std::string const & id, Cell* cell)
    {
        StringCellPair const result = m_cellAreas.equal_range(id);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            if ((*it).second == cell) {
                m_cellAreas.erase(it);
                break;
            }
        }
    }

    void CellCache::removeCellsFromArea(std::string const & id, std::vector<Cell*> const & cells)
    {
        auto it = cells.begin();
        for (; it != cells.end(); ++it) {
            removeCellFromArea(id, *it);
        }
    }

    void CellCache::removeArea(std::string const & id)
    {
        m_cellAreas.erase(id);
    }

    bool CellCache::existsArea(std::string const & id)
    {
        auto it = m_cellAreas.find(id);
        return it != m_cellAreas.end();
    }

    std::vector<std::string> CellCache::getAreas()
    {
        std::vector<std::string> areas;
        std::string last;
        auto it = m_cellAreas.begin();
        for (; it != m_cellAreas.end(); ++it) {
            if (last != (*it).first) {
                last = (*it).first;
                areas.push_back(last);
            }
        }
        return areas;
    }

    std::vector<std::string> CellCache::getCellAreas(Cell* cell)
    {
        std::vector<std::string> areas;
        auto it = m_cellAreas.begin();
        for (; it != m_cellAreas.end(); ++it) {
            if ((*it).second == cell) {
                areas.push_back((*it).first);
            }
        }
        return areas;
    }

    std::vector<Cell*> CellCache::getAreaCells(std::string const & id)
    {
        std::vector<Cell*> cells;
        StringCellPair const result = m_cellAreas.equal_range(id);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            cells.push_back((*it).second);
        }
        return cells;
    }

    bool CellCache::isCellInArea(std::string const & id, Cell* cell)
    {
        StringCellPair const result = m_cellAreas.equal_range(id);
        auto it                     = result.first;
        for (; it != result.second; ++it) {
            if ((*it).second == cell) {
                return true;
            }
        }
        return false;
    }

    Rect CellCache::calculateCurrentSize()
    {
        // set base size
        ModelCoordinate min;
        ModelCoordinate max;
        m_layer->getMinMaxCoordinates(min, max);
        Rect newsize(min.x, min.y, max.x, max.y);

        std::vector<Layer*> const & interacts = m_layer->getInteractLayers();
        if (!interacts.empty()) {
            auto layit = interacts.begin();
            for (; layit != interacts.end(); ++layit) {
                // set size
                (*layit)->getMinMaxCoordinates(min, max, m_layer);
                newsize.w = std::max(max.x, newsize.w);
                newsize.h = std::max(max.y, newsize.h);
                newsize.x = std::min(min.x, newsize.x);
                newsize.y = std::min(min.y, newsize.y);
            }
        }
        return newsize;
    }

    void CellCache::setStaticSize(bool staticSize)
    {
        m_staticSize = staticSize;
    }

    bool CellCache::isStaticSize() const
    {
        return m_staticSize;
    }

    void CellCache::setBlockingUpdate(bool update)
    {
        m_blockingUpdate = update;
    }

    void CellCache::setSizeUpdate(bool update)
    {
        m_sizeUpdate = update;
    }

    void CellCache::update()
    {
        if (m_sizeUpdate) {
            resize();
            m_sizeUpdate = false;
        }
        m_blockingUpdate = false;
    }
} // namespace FIFE
