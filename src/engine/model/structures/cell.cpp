// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <set>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"
#include "view/visual.h"

#include "cell.h"
#include "cellcache.h"
#include "instance.h"
#include "layer.h"

namespace FIFE
{

    static Logger _log(LM_STRUCTURES);

    Cell::Cell(int32_t coordint, const ModelCoordinate& coordinate, Layer* layer) :
        m_coordId(coordint),
        m_coordinate(coordinate),
        m_layer(layer),
        m_zone(nullptr),
        m_transition(nullptr),
        m_type(CTYPE_NO_BLOCKER),
        m_inserted(false),
        m_protect(false)
    {
    }

    Cell::~Cell()
    {
        // calls CellDeleteListener, e.g. for transition
        if (!m_deleteListeners.empty()) {
            auto it = m_deleteListeners.begin();
            for (; it != m_deleteListeners.end(); ++it) {
                if (*it != nullptr) {
                    (*it)->onCellDeleted(this);
                }
            }
        }
        // remove cell from zone
        if (m_zone != nullptr) {
            m_zone->removeCell(this);
        }
        // delete m_transition;
        if (m_transition != nullptr) {
            deleteTransition();
        }
        // remove cell from cache (costs, narrow, area)
        m_layer->getCellCache()->removeCell(this);
    }

    void Cell::addInstances(const std::list<Instance*>& instances)
    {
        CellCache* cache = m_layer->getCellCache();
        for (auto* instance : instances) {
            std::pair<std::set<Instance*>::iterator, bool> const ret = m_instances.insert(instance);
            if (ret.second) {
                if (instance->isSpecialCost()) {
                    cache->registerCost(instance->getCostId(), instance->getCost());
                    cache->addCellToCost(instance->getCostId(), this);
                }
                if (instance->isSpecialSpeed()) {
                    cache->setSpeedMultiplier(this, instance->getSpeed());
                }
                if (!instance->getObject()->getArea().empty()) {
                    cache->addCellToArea(instance->getObject()->getArea(), this);
                }
                callOnInstanceEntered(instance);
            }
        }
        updateCellBlockingInfo();
    }

    void Cell::addInstance(Instance* instance)
    {
        std::pair<std::set<Instance*>::iterator, bool> const ret = m_instances.insert(instance);
        if (ret.second) {
            CellCache* cache = m_layer->getCellCache();
            if (instance->isSpecialCost()) {
                cache->registerCost(instance->getCostId(), instance->getCost());
                cache->addCellToCost(instance->getCostId(), this);
            }
            if (instance->isSpecialSpeed()) {
                cache->setSpeedMultiplier(this, instance->getSpeed());
            }
            if (!instance->getObject()->getArea().empty()) {
                cache->addCellToArea(instance->getObject()->getArea(), this);
            }
            callOnInstanceEntered(instance);
            updateCellBlockingInfo();
        }
    }

    void Cell::changeInstance([[maybe_unused]] Instance* instance)
    {
        updateCellBlockingInfo();
    }

    void Cell::removeInstance(Instance* instance)
    {
        if (m_instances.erase(instance) == 0) {
            FL_ERR(_log, "Tried to remove an instance from cell, but given instance could not be found.");
            return;
        }
        CellCache* cache = m_layer->getCellCache();
        if (instance->isSpecialCost()) {
            cache->removeCellFromCost(instance->getCostId(), this);
        }
        if (instance->isSpecialSpeed()) {
            cache->resetSpeedMultiplier(this);
            // try to find other speed value
            if (!m_instances.empty()) {
                auto it = m_instances.begin();
                for (; it != m_instances.end(); ++it) {
                    if ((*it)->isSpecialSpeed()) {
                        cache->setSpeedMultiplier(this, (*it)->getSpeed());
                        break;
                    }
                }
            }
        }
        if (!instance->getObject()->getArea().empty()) {
            cache->removeCellFromArea(instance->getObject()->getArea(), this);
        }
        callOnInstanceExited(instance);
        updateCellBlockingInfo();
    }

    bool Cell::isNeighbor(Cell* cell)
    {
        auto it = m_neighbors.begin();
        for (; it != m_neighbors.end(); ++it) {
            if (*it == cell) {
                return true;
            }
        }
        return false;
    }

    void Cell::updateCellBlockingInfo()
    {
        CellTypeInfo const old_type = m_type;
        m_coordinate.z              = MIN_CELL_Z;
        if (!m_instances.empty()) {
            int32_t pos          = -1;
            bool const cellblock = (m_type == CTYPE_CELL_NO_BLOCKER || m_type == CTYPE_CELL_BLOCKER);
            for (auto* m_instance : m_instances) {
                if (cellblock) {
                    continue;
                }
                uint8_t const stackpos = m_instance->getCellStackPosition();
                if (std::cmp_less(stackpos, pos)) {
                    continue;
                }
                // update cell z
                if (m_coordinate.z < m_instance->getLocationRef().getLayerCoordinates().z &&
                    m_instance->getObject()->isStatic()) {
                    m_coordinate.z = m_instance->getLocationRef().getLayerCoordinates().z;
                }
                if (std::cmp_greater(m_instance->getCellStackPosition(), pos)) {
                    pos = m_instance->getCellStackPosition();
                    if (m_instance->isBlocking()) {
                        if (!m_instance->getObject()->isStatic()) {
                            m_type = CTYPE_DYNAMIC_BLOCKER;
                        } else {
                            m_type = CTYPE_STATIC_BLOCKER;
                        }
                    } else {
                        m_type = CTYPE_NO_BLOCKER;
                    }
                } else {
                    // if positions are equal then static_blockers win
                    if (m_instance->isBlocking() && m_type != CTYPE_STATIC_BLOCKER) {
                        if (!m_instance->getObject()->isStatic()) {
                            m_type = CTYPE_DYNAMIC_BLOCKER;
                        } else {
                            m_type = CTYPE_STATIC_BLOCKER;
                        }
                    }
                }
            }
        } else {
            if (m_type == CTYPE_STATIC_BLOCKER || m_type == CTYPE_DYNAMIC_BLOCKER) {
                m_type = CTYPE_NO_BLOCKER;
            }
        }
        if (Mathd::Equal(m_coordinate.z, MIN_CELL_Z)) {
            m_coordinate.z = 0;
        }

        if (old_type != m_type) {
            bool const block =
                (m_type == CTYPE_STATIC_BLOCKER || m_type == CTYPE_DYNAMIC_BLOCKER || m_type == CTYPE_CELL_BLOCKER);
            m_layer->getCellCache()->setBlockingUpdate(true);
            callOnBlockingChanged(block);
        }
    }

    void Cell::updateCellInfo()
    {
        updateCellBlockingInfo();

        if (!m_deleteListeners.empty()) {
            m_deleteListeners.erase(std::ranges::remove(m_deleteListeners, nullptr).begin(), m_deleteListeners.end());
        }
        if (!m_changeListeners.empty()) {
            m_changeListeners.erase(std::ranges::remove(m_changeListeners, nullptr).begin(), m_changeListeners.end());
        }
    }

    bool Cell::defaultCost()
    {
        return m_layer->getCellCache()->isDefaultCost(this);
    }

    void Cell::setCostMultiplier(double multi)
    {
        m_layer->getCellCache()->setCostMultiplier(this, multi);
    }

    double Cell::getCostMultiplier()
    {
        return m_layer->getCellCache()->getCostMultiplier(this);
    }

    void Cell::resetCostMultiplier()
    {
        m_layer->getCellCache()->resetCostMultiplier(this);
    }

    bool Cell::defaultSpeed()
    {
        return m_layer->getCellCache()->isDefaultSpeed(this);
    }

    void Cell::setSpeedMultiplier(double multi)
    {
        m_layer->getCellCache()->setSpeedMultiplier(this, multi);
    }

    double Cell::getSpeedMultiplier()
    {
        return m_layer->getCellCache()->getSpeedMultiplier(this);
    }

    void Cell::resetSpeedMultiplier()
    {
        m_layer->getCellCache()->resetSpeedMultiplier(this);
    }

    Zone* Cell::getZone()
    {
        return m_zone;
    }

    void Cell::setZone(Zone* zone)
    {
        m_zone = zone;
    }

    void Cell::resetZone()
    {
        m_inserted = false;
        m_zone     = nullptr;
    }

    bool Cell::isInserted() const
    {
        return m_inserted;
    }

    void Cell::setInserted(bool inserted)
    {
        m_inserted = inserted;
    }

    bool Cell::isZoneProtected() const
    {
        return m_protect;
    }

    void Cell::setZoneProtected(bool protect)
    {
        m_protect = protect;
    }

    CellTypeInfo Cell::getCellType() const
    {
        return m_type;
    }

    void Cell::setCellType(CellTypeInfo type)
    {
        m_type = type;
    }

    const std::set<Instance*>& Cell::getInstances()
    {
        return m_instances;
    }

    void Cell::setCellId(int32_t id)
    {
        m_coordId = id;
    }

    int32_t Cell::getCellId() const
    {
        return m_coordId;
    }

    ModelCoordinate Cell::getLayerCoordinates() const
    {
        return m_coordinate;
    }

    void Cell::addNeighbor(Cell* cell)
    {
        m_neighbors.push_back(cell);
    }

    const std::vector<Cell*>& Cell::getNeighbors()
    {
        return m_neighbors;
    }

    void Cell::resetNeighbors()
    {
        m_neighbors.clear();
        if (m_transition != nullptr) {
            CellCache* cache = m_transition->m_layer->getCellCache();
            if (cache != nullptr) {
                Cell* cell = cache->getCell(m_transition->m_mc);
                if (cell != nullptr) {
                    m_neighbors.push_back(cell);
                }
            }
        }
    }

    Layer* Cell::getLayer()
    {
        return m_layer;
    }

    void Cell::createTransition(Layer* layer, const ModelCoordinate& mc, bool immediate)
    {
        auto* trans = new TransitionInfo(layer);
        // if layers are the same then it's a portal
        if (layer != m_layer) {
            trans->m_difflayer = true;
        }
        trans->m_immediate = immediate;
        trans->m_mc        = mc;

        deleteTransition();

        m_transition = trans;

        Cell* c = layer->getCellCache()->getCell(mc);
        if (c != nullptr) {
            m_neighbors.push_back(c);
            c->addDeleteListener(this);
            m_layer->getCellCache()->addTransition(this);
        } else {
            delete m_transition;
            m_transition = nullptr;
        }
    }

    void Cell::deleteTransition()
    {
        if (m_transition != nullptr) {
            Cell* oldc = m_transition->m_layer->getCellCache()->getCell(m_transition->m_mc);
            auto it    = m_neighbors.begin();
            for (; it != m_neighbors.end(); ++it) {
                if (*it == oldc) {
                    m_neighbors.erase(it);
                    break;
                }
            }
            oldc->removeDeleteListener(this);
            m_layer->getCellCache()->removeTransition(this);
            delete m_transition;
            m_transition = nullptr;
        }
    }

    TransitionInfo* Cell::getTransition()
    {
        return m_transition;
    }

    void Cell::addDeleteListener(CellDeleteListener* listener)
    {
        m_deleteListeners.push_back(listener);
    }

    void Cell::removeDeleteListener(CellDeleteListener* listener)
    {
        auto it = m_deleteListeners.begin();
        for (; it != m_deleteListeners.end(); ++it) {
            if (*it == listener) {
                *it = nullptr;
                break;
            }
        }
    }

    void Cell::onCellDeleted(Cell* cell)
    {
        auto it = m_neighbors.begin();
        for (; it != m_neighbors.end(); ++it) {
            if (*it == cell) {
                deleteTransition();
                break;
            }
        }
    }

    void Cell::addChangeListener(CellChangeListener* listener)
    {
        m_changeListeners.push_back(listener);
    }

    void Cell::removeChangeListener(CellChangeListener* listener)
    {
        auto it = m_changeListeners.begin();
        for (; it != m_changeListeners.end(); ++it) {
            if (*it == listener) {
                *it = nullptr;
                break;
            }
        }
    }

    void Cell::callOnInstanceEntered(Instance* instance)
    {
        if (m_changeListeners.empty()) {
            return;
        }

        auto i = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            if (*i != nullptr) {
                (*i)->onInstanceEnteredCell(this, instance);
            }
            ++i;
        }
    }

    void Cell::callOnInstanceExited(Instance* instance)
    {
        if (m_changeListeners.empty()) {
            return;
        }

        auto i = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            if (*i != nullptr) {
                (*i)->onInstanceExitedCell(this, instance);
            }
            ++i;
        }
    }

    void Cell::callOnBlockingChanged(bool blocks)
    {
        if (m_changeListeners.empty()) {
            return;
        }

        auto i = m_changeListeners.begin();
        while (i != m_changeListeners.end()) {
            if (*i != nullptr) {
                (*i)->onBlockingChangedCell(this, m_type, blocks);
            }
            ++i;
        }
    }
} // namespace FIFE
