// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/modelcoords.h"
#include "util/log/logger.h"

#include "cell.h"
#include "cellcache.h"
#include "map.h"
#include "trigger.h"
#include "triggercontroller.h"

namespace FIFE
{

    static Logger _log(LM_STRUCTURES);

    TriggerController::TriggerController(Map* map) : m_map(map) { }

    TriggerController::~TriggerController()
    {
        auto it = m_triggerNameMap.begin();
        for (; it != m_triggerNameMap.end(); ++it) {
            delete it->second;
        }
    }

    Trigger* TriggerController::createTrigger(const std::string& triggerName)
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
        const std::string& triggerName, Layer* layer, const ModelCoordinate& pt)
    {
        assert(layer);

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(layer, pt);
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCoordinates(
        const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords)
    {
        assert(layer);

        Trigger* trigger = createTrigger(triggerName);
        for (const auto& coord : coords) {
            trigger->assign(layer, coord);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnRect(const std::string& triggerName, Layer* layer, const Rect& rec)
    {
        assert(layer);
        assert(layer->getCellCache());

        Trigger* trigger         = createTrigger(triggerName);
        std::vector<Cell*> cells = layer->getCellCache()->getCellsInRect(rec);
        for (auto& cell : cells) {
            trigger->assign(cell);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnLocation(const std::string& triggerName, const Location& loc)
    {
        assert(loc.getLayer());
        assert(loc.getLayer()->getCellCache());

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(loc.getLayer(), loc.getLayerCoordinates());
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnLocations(
        const std::string& triggerName, const std::vector<Location>& locs)
    {
        Trigger* trigger = createTrigger(triggerName);
        for (const auto& loc : locs) {
            trigger->assign(loc.getLayer(), loc.getLayerCoordinates());
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCell(const std::string& triggerName, Cell* cell)
    {
        assert(cell);

        Trigger* trigger = createTrigger(triggerName);
        trigger->assign(cell);
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnCells(const std::string& triggerName, const std::vector<Cell*>& cells)
    {
        Trigger* trigger = createTrigger(triggerName);
        for (auto cell : cells) {
            trigger->assign(cell);
        }
        return trigger;
    }

    Trigger* TriggerController::createTriggerOnInstance(const std::string& triggerName, Instance* instance)
    {
        assert(instance);

        Trigger* trigger = createTrigger(triggerName);
        trigger->attach(instance);
        return trigger;
    }

    Trigger* TriggerController::getTrigger(const std::string& triggerName)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            return it->second;
        }
        return nullptr;
    }

    void TriggerController::deleteTrigger(const std::string& triggerName)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            delete it->second;
            m_triggerNameMap.erase(it);
        }
    }

    void TriggerController::removeTriggerFromCoordinate(
        const std::string& triggerName, Layer* layer, const ModelCoordinate& pt)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(layer, pt);
        }
    }

    void TriggerController::removeTriggerFromCoordinates(
        const std::string& triggerName, Layer* layer, const std::vector<ModelCoordinate>& coords)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (const auto& coord : coords) {
                it->second->remove(layer, coord);
            }
        }
    }

    void TriggerController::removeTriggerFromRect(const std::string& triggerName, Layer* layer, const Rect& rec)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            std::vector<Cell*> cells = layer->getCellCache()->getCellsInRect(rec);
            for (auto& cell : cells) {
                it->second->remove(cell);
            }
        }
    }

    void TriggerController::removeTriggerFromLocation(const std::string& triggerName, const Location& loc)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(loc.getLayer(), loc.getLayerCoordinates());
        }
    }

    void TriggerController::removeTriggerFromLocations(
        const std::string& triggerName, const std::vector<Location>& locs)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (const auto& loc : locs) {
                it->second->remove(loc.getLayer(), loc.getLayerCoordinates());
            }
        }
    }

    void TriggerController::removeTriggerFromCell(const std::string& triggerName, Cell* cell)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            it->second->remove(cell);
        }
    }

    void TriggerController::removeTriggerFromCells(const std::string& triggerName, const std::vector<Cell*>& cells)
    {
        auto it = m_triggerNameMap.find(triggerName);
        if (it != m_triggerNameMap.end()) {
            for (auto cell : cells) {
                it->second->remove(cell);
            }
        }
    }

    void TriggerController::removeTriggerFromInstance(const std::string& triggerName, Instance* instance)
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

    bool TriggerController::exists(const std::string& name)
    {
        auto it = m_triggerNameMap.find(name);
        if (it != m_triggerNameMap.end()) {
            return true;
        }

        return false;
    }

} // namespace FIFE
