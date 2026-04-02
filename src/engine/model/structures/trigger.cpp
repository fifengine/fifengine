// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "cell.h"
#include "cellcache.h"
#include "instance.h"
#include "layer.h"
#include "location.h"
#include "trigger.h"

namespace FIFE
{

    class TriggerChangeListener :
        public CellChangeListener,
        public InstanceChangeListener,
        public InstanceDeleteListener
    {
    public:
        explicit TriggerChangeListener(Trigger* trigger) : m_trigger(trigger) { }
        ~TriggerChangeListener() override = default;

        // InstanceDeleteListener callback
        void onInstanceDeleted(Instance* instance) override
        {
            const std::vector<TriggerCondition>& types = m_trigger->getTriggerConditions();
            if (std::ranges::find(types, INSTANCE_TRIGGER_DELETE) != types.end()) {
                m_trigger->setTriggered();
            }
            m_trigger->detach();
        }

        // CellChangeListener callback
        void onInstanceEnteredCell(Cell* cell, Instance* instance) override
        {
            const std::vector<TriggerCondition>& types = m_trigger->getTriggerConditions();
            if (std::ranges::find(types, CELL_TRIGGER_ENTER) != types.end()) {
                const std::vector<Instance*>& restrict = m_trigger->getEnabledInstances();
                if (m_trigger->isEnabledForAllInstances() || std::ranges::find(restrict, instance) != restrict.end()) {
                    m_trigger->setTriggered();
                }
            }
        }

        // CellChangeListener callback
        void onInstanceExitedCell(Cell* cell, Instance* instance) override
        {
            const std::vector<TriggerCondition>& types = m_trigger->getTriggerConditions();
            if (std::ranges::find(types, CELL_TRIGGER_EXIT) != types.end()) {
                const std::vector<Instance*>& restrict = m_trigger->getEnabledInstances();
                if (m_trigger->isEnabledForAllInstances() || std::ranges::find(restrict, instance) != restrict.end()) {
                    m_trigger->setTriggered();
                }
            }
        }

        // CellChangeListener callback
        void onBlockingChangedCell(Cell* cell, CellTypeInfo type, bool blocks) override
        {
            const std::vector<TriggerCondition>& types = m_trigger->getTriggerConditions();
            if (std::ranges::find(types, CELL_TRIGGER_BLOCKING_CHANGE) != types.end()) {
                m_trigger->setTriggered();
            }
        }

        // InstanceChangeListener callback
        void onInstanceChanged(Instance* instance, InstanceChangeInfo info) override
        {
            const std::vector<TriggerCondition>& types = m_trigger->getTriggerConditions();
            if (m_trigger->getAttached() == instance && (info & ICHANGE_CELL) == ICHANGE_CELL) {
                m_trigger->move();
            }

            if (types.empty()) {
                return;
            }

            const auto hasTrigger = [&](InstanceChangeInfo change, TriggerCondition condition) {
                return (info & change) == change && std::ranges::find(types, condition) != types.end();
            };

            if (hasTrigger(ICHANGE_LOC, INSTANCE_TRIGGER_LOCATION) ||
                hasTrigger(ICHANGE_ROTATION, INSTANCE_TRIGGER_ROTATION) ||
                hasTrigger(ICHANGE_SPEED, INSTANCE_TRIGGER_SPEED) ||
                hasTrigger(ICHANGE_ACTION, INSTANCE_TRIGGER_ACTION) ||
                hasTrigger(ICHANGE_TIME_MULTIPLIER, INSTANCE_TRIGGER_TIME_MULTIPLIER) ||
                hasTrigger(ICHANGE_SAYTEXT, INSTANCE_TRIGGER_SAYTEXT) ||
                hasTrigger(ICHANGE_BLOCK, INSTANCE_TRIGGER_BLOCK) || hasTrigger(ICHANGE_CELL, INSTANCE_TRIGGER_CELL) ||
                hasTrigger(ICHANGE_TRANSPARENCY, INSTANCE_TRIGGER_TRANSPARENCY) ||
                hasTrigger(ICHANGE_VISIBLE, INSTANCE_TRIGGER_VISIBLE) ||
                hasTrigger(ICHANGE_STACKPOS, INSTANCE_TRIGGER_STACKPOS) ||
                hasTrigger(ICHANGE_VISUAL, INSTANCE_TRIGGER_VISUAL)) {
                m_trigger->setTriggered();
            }
        }

    private:
        Trigger* m_trigger;
    };

    Trigger::Trigger() :
        m_triggered(false), m_enabledAll(false), m_attached(nullptr), m_changeListener(new TriggerChangeListener(this))
    {
    }

    Trigger::Trigger(std::string name) :
        m_name(std::move(name)),
        m_triggered(false),
        m_enabledAll(false),
        m_attached(nullptr),
        m_changeListener(new TriggerChangeListener(this))
    {
    }

    Trigger::~Trigger()
    {
        detach();
        auto it = m_assigned.begin();
        for (; it != m_assigned.end(); ++it) {
            (*it)->removeChangeListener(m_changeListener);
        }
        delete m_changeListener;
    }

    void Trigger::addTriggerListener(ITriggerListener* listener)
    {
        auto it = std::ranges::find(m_triggerListeners, listener);
        if (it == m_triggerListeners.end()) {
            m_triggerListeners.push_back(listener);
        }
    }

    void Trigger::removeTriggerListener(ITriggerListener* listener)
    {
        auto i = m_triggerListeners.begin();
        while (i != m_triggerListeners.end()) {
            if ((*i) == listener) {
                // set the pointer to null, so it can be removed later
                *i = nullptr;
                return;
            }
            ++i;
        }
    }

    void Trigger::reset()
    {
        m_triggered = false;
    }

    void Trigger::setTriggered()
    {
        if (!m_triggered) {
            m_triggered = true;
            auto i      = m_triggerListeners.begin();
            while (i != m_triggerListeners.end()) {
                if (*i != nullptr) {
                    (*i)->onTriggered();
                }
                ++i;
            }
        }
        // remove null pointer
        m_triggerListeners.erase(std::ranges::remove(m_triggerListeners, nullptr).begin(), m_triggerListeners.end());
    }

    void Trigger::addTriggerCondition(TriggerCondition type)
    {
        auto it = std::ranges::find(m_triggerConditions, type);
        if (it == m_triggerConditions.end()) {
            m_triggerConditions.push_back(type);
        }
    }

    const std::vector<TriggerCondition>& Trigger::getTriggerConditions()
    {
        return m_triggerConditions;
    }

    void Trigger::removeTriggerCondition(TriggerCondition type)
    {
        auto it = std::ranges::find(m_triggerConditions, type);
        if (it != m_triggerConditions.end()) {
            m_triggerConditions.erase(it);
        }
    }

    void Trigger::enableForInstance(Instance* instance)
    {
        auto it = std::ranges::find(m_enabledInstances, instance);
        if (it == m_enabledInstances.end()) {
            m_enabledInstances.push_back(instance);
        }
    }

    const std::vector<Instance*>& Trigger::getEnabledInstances()
    {
        return m_enabledInstances;
    }

    void Trigger::disableForInstance(Instance* instance)
    {
        auto it = std::ranges::find(m_enabledInstances, instance);
        if (it != m_enabledInstances.end()) {
            m_enabledInstances.erase(it);
        }
    }

    void Trigger::enableForAllInstances()
    {
        m_enabledAll = true;
    }

    bool Trigger::isEnabledForAllInstances() const
    {
        return m_enabledAll;
    }

    void Trigger::disableForAllInstances()
    {
        m_enabledAll = false;
    }

    void Trigger::assign(Layer* layer, const ModelCoordinate& pt)
    {
        Cell* cell = layer->getCellCache()->getCell(pt);
        if (cell == nullptr) {
            return;
        }
        auto it = std::ranges::find(m_assigned, cell);
        if (it == m_assigned.end()) {
            m_assigned.push_back(cell);
            cell->addChangeListener(m_changeListener);
        }
    }

    void Trigger::remove(Layer* layer, const ModelCoordinate& pt)
    {
        Cell* cell = layer->getCellCache()->getCell(pt);
        if (cell == nullptr) {
            return;
        }
        auto it = std::ranges::find(m_assigned, cell);
        if (it != m_assigned.end()) {
            m_assigned.erase(it);
            cell->removeChangeListener(m_changeListener);
        }
    }

    void Trigger::assign(Cell* cell)
    {
        auto it = std::ranges::find(m_assigned, cell);
        if (it == m_assigned.end()) {
            m_assigned.push_back(cell);
            cell->addChangeListener(m_changeListener);
        }
    }

    void Trigger::remove(Cell* cell)
    {
        auto it = std::ranges::find(m_assigned, cell);
        if (it != m_assigned.end()) {
            m_assigned.erase(it);
            cell->removeChangeListener(m_changeListener);
        }
    }

    const std::vector<Cell*>& Trigger::getAssignedCells()
    {
        return m_assigned;
    }

    void Trigger::attach(Instance* instance)
    {
        if (instance == m_attached) {
            return;
        }

        if (m_attached != nullptr) {
            detach();
        }
        m_attached = instance;
        m_attached->addDeleteListener(m_changeListener);
        m_attached->addChangeListener(m_changeListener);
    }

    void Trigger::detach()
    {
        if (m_attached != nullptr) {
            m_attached->removeDeleteListener(m_changeListener);
            m_attached->removeChangeListener(m_changeListener);
            m_attached = nullptr;
        }
    }

    void Trigger::move()
    {
        if (m_assigned.empty()) {
            return;
        }
        const ModelCoordinate newPos = m_attached->getLocationRef().getLayerCoordinates();
        const ModelCoordinate oldPos = m_attached->getOldLocationRef().getLayerCoordinates();
        moveTo(newPos, oldPos);
    }

    void Trigger::moveTo(const ModelCoordinate& newPos, const ModelCoordinate& oldPos)
    {
        const ModelCoordinate mc(newPos.x - oldPos.x, newPos.y - oldPos.y);

        CellCache* cache = m_attached->getLocationRef().getLayer()->getCellCache();
        std::vector<Cell*> newCells;
        auto it = m_assigned.begin();
        for (; it != m_assigned.end(); ++it) {
            ModelCoordinate coord = (*it)->getLayerCoordinates();
            coord.x += mc.x;
            coord.y += mc.y;
            Cell* c = cache->getCell(coord);
            if (c != nullptr) {
                newCells.push_back(c);
            }
        }
        for (it = newCells.begin(); it != newCells.end(); ++it) {
            auto found = std::ranges::find(m_assigned, *it);
            if (found != m_assigned.end()) {
                m_assigned.erase(found);
            } else {
                // add new
                (*it)->addChangeListener(m_changeListener);
            }
        }
        // remove old
        for (it = m_assigned.begin(); it != m_assigned.end(); ++it) {
            (*it)->removeChangeListener(m_changeListener);
        }
        m_assigned = newCells;
    }
} // namespace FIFE
