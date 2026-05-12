// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "triggercontroller.h"

// Standard C++ library includes
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "cell.h"
#include "cellcache.h"
#include "map.h"
#include "model/metamodel/modelcoords.h"
#include "trigger.h"
#include "util/log/logger.h"

namespace FIFE
{

    static Logger _log(LM_STRUCTURES);

    TriggerController::TriggerController(Map* map) : m_map(map)
    {
    }

    TriggerController::~TriggerController()
    {
        auto it = m_triggerNameMap.begin();
        for (; it != m_triggerNameMap.end(); ++it) {
            delete it->second;
        }
    }

    Trigger* TriggerController::createTrigger(std::string const & triggerName)
    {
        // assert(!exists(triggerName));

        auto* trigger = new Trigger(triggerName);

        std::pair<TriggerNameMapIterator, bool> returnValue;
        returnValue = m_triggerNameMap.insert(TriggerNameMapPair(triggerName, trigger));

        if (!returnValue.second) {
            delete trigger;
            FL_WARN(
                _log,
                LMsg("TriggerController::createTrigger() - ")
                    << "Trigger " << triggerName << " already exists.... ignoring.");
        }

        return returnValue.first->second;
    }

    Trigger* TriggerController::createTriggerOnCoordinate(
        std::string const & triggerName, Layer* layer, ModelCoordinate const & pt)
    {
        assert(layer);

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(layer, pt);
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCoordinates(
        std::string const & triggerName, Layer* layer, std::vector<ModelCoordinate> const & coords)
    {
        assert(layer);

        Trigger* trigger = createTrigger(triggerName);
        for (auto const & coord : coords) {
            trigger->assign(layer, coord);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnRect(std::string const & triggerName, Layer* layer, Rect const & rec)
    {
        assert(layer);
        CellCache* cellCache = layer->getCellCache();
        assert(cellCache);

        Trigger* trigger               = createTrigger(triggerName);
        std::vector<Cell*> const cells = cellCache->getCellsInRect(rec);
        for (auto const & cell : cells) {
            trigger->assign(cell);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnLocation(std::string const & triggerName, Location const & loc)
    {
        assert(loc.getLayer());
        assert(loc.getLayer()->getCellCache());

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(loc.getLayer(), loc.getLayerCoordinates());
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnLocations(
        std::string const & triggerName, std::vector<Location> const & locs)
    {
        Trigger* trigger = createTrigger(triggerName);
        for (auto const & loc : locs) {
            trigger->assign(loc.getLayer(), loc.getLayerCoordinates());
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCell(std::string const & triggerName, Cell* cell)
    {
        assert(cell);

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(cell);
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCells(std::string const & triggerName, std::vector<Cell*> const & cells)
    {
        Trigger* trigger = createTrigger(triggerName);
        for (auto* cell : cells) {
            trigger->assign(cell);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnInstance(std::string const & triggerName, Instance* instance)
    {
        assert(instance);

        Trigger* trigger = createTrigger(triggerName);
        trigger->attach(instance);
        return trigger;
    }

    Trigger* TriggerController::getTrigger(std::string const & triggerName)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void TriggerController::deleteTrigger(std::string const & triggerName)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            delete it->second;
            m_triggerNameMap.erase(it);
        }
    }

    void TriggerController::removeTriggerFromCoordinate(
        std::string const & triggerName, Layer* layer, ModelCoordinate const & pt)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(layer, pt);
        }
    }

    void TriggerController::removeTriggerFromCoordinates(
        std::string const & triggerName, Layer* layer, std::vector<ModelCoordinate> const & coords)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (auto const & coord : coords) {
                it->second->remove(layer, coord);
            }
        }
    }

    void TriggerController::removeTriggerFromRect(std::string const & triggerName, Layer* layer, Rect const & rec)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            std::vector<Cell*> const cells = layer->getCellCache()->getCellsInRect(rec);
            for (auto const & cell : cells) {
                it->second->remove(cell);
            }
        }
    }

    void TriggerController::removeTriggerFromLocation(std::string const & triggerName, Location const & loc)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(loc.getLayer(), loc.getLayerCoordinates());
        }
    }

    void TriggerController::removeTriggerFromLocations(
        std::string const & triggerName, std::vector<Location> const & locs)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (auto const & loc : locs) {
                it->second->remove(loc.getLayer(), loc.getLayerCoordinates());
            }
        }
    }

    void TriggerController::removeTriggerFromCell(std::string const & triggerName, Cell* cell)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(cell);
        }
    }

    void TriggerController::removeTriggerFromCells(std::string const & triggerName, std::vector<Cell*> const & cells)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (auto* cell : cells) {
                it->second->remove(cell);
            }
        }
    }

    void TriggerController::removeTriggerFromInstance(std::string const & triggerName, Instance* instance)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            if (it->second->getAttached() == instance) {
                it->second->detach();
            }
        }
    }

    std::vector<Trigger*> TriggerController::getAllTriggers()
    {
        std::vector<Trigger*> triggers;
        auto it = m_triggerNameMap.begin();
        for (; it != m_triggerNameMap.end(); ++it) {
            triggers.push_back(it->second);
        }
        return triggers;
    }

    std::vector<std::string> TriggerController::getAllTriggerNames()
    {
        std::vector<std::string> names;
        auto it = m_triggerNameMap.begin();
        for (; it != m_triggerNameMap.end(); ++it) {
            names.push_back(it->first);
        }
        return names;
    }

    bool TriggerController::exists(std::string const & name)
    {
        auto it = m_triggerNameMap.find(name);
        return it != m_triggerNameMap.end();
    }

} // namespace FIFE
