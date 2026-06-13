// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "object.h"

// Standard C++ library includes
#include <cassert>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "action.h"
#include "grids/cellgrid.h"
#include "ipather.h"
#include "util/base/exception.h"

namespace FIFE
{
    namespace
    {
        [[nodiscard]] uint32_t toAngleKey(int32_t const rotation)
        {
            assert(rotation >= 0);
            return static_cast<uint32_t>(rotation);
        }
    } // namespace

    Object::BasicObjectProperty::BasicObjectProperty() :
        m_defaultAction(nullptr), m_blocking(false), m_static(false), m_cellStack(0)
    {
    }

    Object::BasicObjectProperty::~BasicObjectProperty() = default;

    namespace
    {
        // deleter for Action used by unique_ptr<Action, void(*)(Action*)>
        void action_deleter(Action* a)
        {
            delete a; // NOLINT(cppcoreguidelines-owning-memory)
        }
    } // namespace

    Object::MovableObjectProperty::MovableObjectProperty() : m_pather(nullptr), m_cost(1.0), m_speed(1.0), m_zRange(0)
    {
    }

    Object::MultiObjectProperty::MultiObjectProperty() : m_multiPart(false), m_restrictedRotation(false)
    {
    }

    Object::Object(std::string identifier, std::string name_space, Object* inherited) :
        m_name(std::move(identifier)), m_namespace(std::move(name_space)), m_inherited(inherited)
    {
    }

    Object::~Object() = default;

    Action* Object::createAction(std::string const & identifier, bool is_default)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = std::make_unique<BasicObjectProperty>();
        }

        auto it = m_basicProperty->m_actions.find(identifier);
        if (it != m_basicProperty->m_actions.end()) {
            throw NameClash(identifier);
        }

        std::unique_ptr<Action, void (*)(Action*)> a(new Action(identifier), &action_deleter);

        Action* raw = a.get();

        m_basicProperty->m_actions.emplace(identifier, std::move(a));

        if (is_default || (m_basicProperty->m_defaultAction == nullptr)) {
            m_basicProperty->m_defaultAction = raw;
        }

        // a is moved into the map at L84.
        // raw points to the now-map-owned object and not a dangling local.
        return raw; // cppcheck-suppress returnDanglingLifetime
    }

    Action* Object::getAction(std::string const & identifier, bool deepsearch) const
    {
        if (m_basicProperty != nullptr) {
            auto i = m_basicProperty->m_actions.find(identifier);
            if (i != m_basicProperty->m_actions.end()) {
                return i->second.get();
            }
        }
        if ((m_inherited != nullptr) && deepsearch) {
            return m_inherited->getAction(identifier);
        }
        return nullptr;
    }

    std::list<std::string> Object::getActionIds() const
    {
        std::list<std::string> action_ids;
        if (m_basicProperty != nullptr) {
            for (auto const& [id, _] : m_basicProperty->m_actions) {
                action_ids.push_back(id);
            }
        }
        return action_ids;
    }

    void Object::setDefaultAction(std::string const & identifier)
    {
        Action* action = nullptr;
        if (m_basicProperty != nullptr) {
            auto i = m_basicProperty->m_actions.find(identifier);
            if (i != m_basicProperty->m_actions.end()) {
                action = i->second.get();
            }
        }
        if (action == nullptr && m_inherited != nullptr) {
            action = m_inherited->getAction(identifier);
        }

        if (action != nullptr && m_basicProperty != nullptr) {
            m_basicProperty->m_defaultAction = action;
        }
    }

    Action* Object::getDefaultAction() const
    {
        if (m_basicProperty != nullptr) {
            return m_basicProperty->m_defaultAction;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getDefaultAction();
        }
        return nullptr;
    }

    void Object::setPather(IPather* pather)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_pather = pather;
    }

    IPather* Object::getPather() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_pather;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getPather();
        }
        return nullptr;
    }

    Object* Object::getInherited() const
    {
        return m_inherited;
    }

    void Object::adoptVisual(IVisual* visual) // cppcheck-suppress constParameterPointer
    {
        if (m_visual && m_visual.get() != visual) {
            // m_visual will be reset below, old one auto-destructs
        }
        m_visual.reset(visual);
    }

    void Object::setBlocking(bool blocking)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = std::make_unique<BasicObjectProperty>();
        }
        m_basicProperty->m_blocking = blocking;
    }

    bool Object::isBlocking() const
    {
        if (m_basicProperty != nullptr) {
            return m_basicProperty->m_blocking;
        }
        if (m_inherited != nullptr) {
            return m_inherited->isBlocking();
        }
        return false;
    }

    void Object::setStatic(bool stat)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = std::make_unique<BasicObjectProperty>();
        }
        m_basicProperty->m_static = stat;
    }

    bool Object::isStatic() const
    {
        if (m_basicProperty == nullptr) {
            if (m_inherited != nullptr) {
                return m_inherited->isStatic();
            }
            return false;
        }
        return m_basicProperty->m_static;
    }

    void Object::setFilename(std::string const & file)
    {
        m_filename = file;
    }

    std::string const & Object::getFilename() const
    {
        return m_filename;
    }

    void Object::setCellStackPosition(uint8_t position)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = std::make_unique<BasicObjectProperty>();
        }
        m_basicProperty->m_cellStack = position;
    }

    uint8_t Object::getCellStackPosition() const
    {
        if (m_basicProperty != nullptr) {
            return m_basicProperty->m_cellStack;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getCellStackPosition();
        }
        return 0;
    }

    bool Object::isSpecialCost() const
    {
        if (m_moveProperty != nullptr) {
            return !m_moveProperty->m_costId.empty();
        }
        if (m_inherited != nullptr) {
            return m_inherited->isSpecialCost();
        }
        return false;
    }

    void Object::setCostId(std::string const & cost)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_costId = cost;
    }

    std::string Object::getCostId() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_costId;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getCostId();
        }
        return "";
    }

    void Object::setCost(double cost)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_cost = cost;
    }

    double Object::getCost() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_cost;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getCost();
        }
        return 1.0;
    }

    bool Object::isSpecialSpeed() const
    {
        if (m_moveProperty != nullptr) {
            return !Mathd::Equal(m_moveProperty->m_speed, 1.0);
        }
        if (m_inherited != nullptr) {
            return m_inherited->isSpecialSpeed();
        }
        return false;
    }

    void Object::setSpeed(double speed)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_speed = speed;
    }

    double Object::getSpeed() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_speed;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getSpeed();
        }
        return 1.0;
    }

    bool Object::isMultiObject() const
    {
        if (m_multiProperty != nullptr) {
            return !m_multiProperty->m_multiPartIds.empty();
        }
        if (m_inherited != nullptr) {
            return m_inherited->isMultiObject();
        }
        return false;
    }

    void Object::addMultiPartId(std::string const & partId)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_multiPartIds.push_back(partId);
    }

    std::list<std::string> Object::getMultiPartIds() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_multiPartIds;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getMultiPartIds();
        }
        return {};
    }

    void Object::removeMultiPartId(std::string const & partId)
    {
        if (m_multiProperty == nullptr) {
            return;
        }
        auto it = m_multiProperty->m_multiPartIds.begin();
        for (; it != m_multiProperty->m_multiPartIds.end(); ++it) {
            if (*it == partId) {
                m_multiProperty->m_multiPartIds.erase(it);
                break;
            }
        }
    }

    void Object::removeAllMultiPartIds()
    {
        if (m_multiProperty == nullptr) {
            return;
        }
        m_multiProperty->m_multiPartIds.clear();
    }

    bool Object::isMultiPart() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_multiPart;
        }
        if (m_inherited != nullptr) {
            return m_inherited->isMultiPart();
        }
        return false;
    }

    void Object::setMultiPart(bool part)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_multiPart = part;
    }

    void Object::addMultiPart(Object* obj)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_multiParts.insert(obj);
    }

    std::set<Object*> Object::getMultiParts() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_multiParts;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getMultiParts();
        }
        return {};
    }

    void Object::removeMultiPart(Object* obj)
    {
        if (m_multiProperty == nullptr) {
            return;
        }
        m_multiProperty->m_multiParts.erase(obj);
    }

    void Object::removeMultiParts()
    {
        if (m_multiProperty == nullptr) {
            return;
        }
        m_multiProperty->m_multiParts.clear();
    }

    void Object::addMultiPartCoordinate(int32_t rotation, ModelCoordinate const & coord)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_multiPartCoordinates.insert(std::pair<int32_t, ModelCoordinate>(rotation, coord));
        m_multiProperty->m_partAngleMap[toAngleKey(rotation)] = rotation;
    }

    std::multimap<int32_t, ModelCoordinate> Object::getMultiPartCoordinates() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_multiPartCoordinates;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getMultiPartCoordinates();
        }
        return {};
    }

    std::vector<ModelCoordinate> Object::getMultiPartCoordinates(int32_t rotation) const
    {
        std::vector<ModelCoordinate> coordinates;

        if (m_multiProperty != nullptr) {
            int32_t closest = 0;
            getIndexByAngle(rotation, m_multiProperty->m_partAngleMap, closest);
            std::pair<
                std::multimap<int32_t, ModelCoordinate>::iterator,
                std::multimap<int32_t, ModelCoordinate>::iterator> const result =
                m_multiProperty->m_multiPartCoordinates.equal_range(closest);
            auto it = result.first;
            for (; it != result.second; ++it) {
                coordinates.push_back((*it).second);
            }
        } else if (m_inherited != nullptr) {
            return m_inherited->getMultiPartCoordinates(rotation);
        }
        return coordinates;
    }

    std::vector<ModelCoordinate> Object::getMultiObjectCoordinates(int32_t rotation) const
    {
        std::vector<ModelCoordinate> coordinates;
        if (m_multiProperty != nullptr) {
            if (m_multiProperty->m_multiObjectCoordinates.empty()) {
                auto subit = m_multiProperty->m_multiParts.begin();
                for (; subit != m_multiProperty->m_multiParts.end(); ++subit) {
                    std::multimap<int32_t, ModelCoordinate> const & subcoords = (*subit)->getMultiPartCoordinates();
                    m_multiProperty->m_multiObjectCoordinates.insert(subcoords.begin(), subcoords.end());
                }
                auto it = m_multiProperty->m_multiObjectCoordinates.begin();
                for (; it != m_multiProperty->m_multiObjectCoordinates.end(); ++it) {
                    m_multiProperty->m_multiAngleMap[toAngleKey((*it).first)] = (*it).first;
                }
            }
            int32_t closest = 0;
            getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
            std::pair<
                std::multimap<int32_t, ModelCoordinate>::iterator,
                std::multimap<int32_t, ModelCoordinate>::iterator> const result =
                m_multiProperty->m_multiObjectCoordinates.equal_range(closest);
            auto it = result.first;
            ModelCoordinate const parent(0, 0);
            coordinates.push_back(parent);
            for (; it != result.second; ++it) {
                coordinates.push_back((*it).second);
            }
        } else if (m_inherited != nullptr) {
            return m_inherited->getMultiObjectCoordinates(rotation);
        }
        return coordinates;
    }

    void Object::setRotationAnchor(ExactModelCoordinate const & anchor)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_rotationAnchor = anchor;
    }

    ExactModelCoordinate Object::getRotationAnchor() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_rotationAnchor;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getRotationAnchor();
        }
        return ExactModelCoordinate();
    }

    void Object::setRestrictedRotation(bool restrict)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        m_multiProperty->m_restrictedRotation = restrict;
    }

    void Object::initializeFootprintCache(CellGrid* grid)
    {
        if (m_multiProperty == nullptr || grid == nullptr) {
            return;
        }
        // Precompute footprint for each distinct rotation angle
        for (auto const & angleEntry : m_multiProperty->m_multiAngleMap) {
            int32_t const rot = angleEntry.second;
            std::vector<ModelCoordinate> offsets =
                grid->toMultiCoordinates(ModelCoordinate(0, 0, 0), getMultiObjectCoordinates(rot));
            assert("cached footprint must not be empty for valid rotation" && !offsets.empty());
            m_multiProperty->m_footprintCache[rot] = std::move(offsets);
        }
        // Also cache rotation 0 if not present (every multi-cell defines rot 0)
        if (!m_multiProperty->m_footprintCache.contains(0)) {
            std::vector<ModelCoordinate> offsets =
                grid->toMultiCoordinates(ModelCoordinate(0, 0, 0), getMultiObjectCoordinates(0));
            m_multiProperty->m_footprintCache[0] = std::move(offsets);
        }
    }

    std::vector<ModelCoordinate> const & Object::getCachedFootprint(int32_t rotation) const
    {
        static std::vector<ModelCoordinate> const empty;
        if (m_multiProperty != nullptr) {
            auto it = m_multiProperty->m_footprintCache.find(rotation);
            if (it != m_multiProperty->m_footprintCache.end()) {
                return it->second;
            }
            // Fallback to closest matching angle
            int32_t closest = rotation;
            if (!m_multiProperty->m_multiAngleMap.empty()) {
                getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
            }
            it = m_multiProperty->m_footprintCache.find(closest);
            if (it != m_multiProperty->m_footprintCache.end()) {
                return it->second;
            }
        }
        if (m_inherited != nullptr) {
            return m_inherited->getCachedFootprint(rotation);
        }
        return empty;
    }

    void Object::setFootprintCellCost(int32_t rotation, size_t cellIndex, double cost)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = std::make_unique<MultiObjectProperty>();
        }
        assert("footprint cell cost must be non-negative" && cost >= 0.0);
        // Ensure the cost vector exists for this rotation
        auto fit = m_multiProperty->m_footprintCache.find(rotation);
        if (fit != m_multiProperty->m_footprintCache.end() && cellIndex < fit->second.size()) {
            auto& costs = m_multiProperty->m_footprintCosts[rotation];
            if (costs.size() <= cellIndex) {
                costs.resize(fit->second.size(), 1.0);
            }
            costs.at(cellIndex) = cost;
        }
    }

    double Object::getFootprintCellCost(int32_t rotation, size_t cellIndex) const
    {
        if (m_multiProperty != nullptr) {
            auto it = m_multiProperty->m_footprintCosts.find(rotation);
            if (it != m_multiProperty->m_footprintCosts.end() && cellIndex < it->second.size()) {
                return it->second.at(cellIndex);
            }
        }
        if (m_inherited != nullptr) {
            return m_inherited->getFootprintCellCost(rotation, cellIndex);
        }
        return 1.0;
    }

    bool Object::hasFootprintCosts() const
    {
        if (m_multiProperty != nullptr) {
            return !m_multiProperty->m_footprintCosts.empty();
        }
        if (m_inherited != nullptr) {
            return m_inherited->hasFootprintCosts();
        }
        return false;
    }

    bool Object::isRestrictedRotation() const
    {
        if (m_multiProperty != nullptr) {
            return m_multiProperty->m_restrictedRotation;
        }
        if (m_inherited != nullptr) {
            return m_inherited->isRestrictedRotation();
        }
        return false;
    }

    int32_t Object::getRestrictedRotation(int32_t rotation)
    {
        int32_t closest = rotation;
        if (m_multiProperty != nullptr) {
            if (!m_multiProperty->m_multiAngleMap.empty()) {
                getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
            } else if (!m_multiProperty->m_partAngleMap.empty()) {
                getIndexByAngle(rotation, m_multiProperty->m_partAngleMap, closest);
            }
        } else if (m_inherited != nullptr) {
            return m_inherited->getRestrictedRotation(rotation);
        }
        return closest;
    }

    void Object::setZStepRange(int32_t zRange)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_zRange = zRange;
    }

    int32_t Object::getZStepRange() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_zRange;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getZStepRange();
        }
        return 0;
    }

    void Object::setArea(std::string const & id)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = std::make_unique<BasicObjectProperty>();
        }
        m_basicProperty->m_area = id;
    }

    std::string Object::getArea() const
    {
        if (m_basicProperty != nullptr) {
            return m_basicProperty->m_area;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getArea();
        }
        return "";
    }

    void Object::addWalkableArea(std::string const & id)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = std::make_unique<MovableObjectProperty>();
        }
        m_moveProperty->m_walkableAreas.push_back(id);
        m_moveProperty->m_walkableAreas.sort();
        m_moveProperty->m_walkableAreas.unique();
    }

    void Object::removeWalkableArea(std::string const & id)
    {
        if (m_moveProperty == nullptr) {
            return;
        }
        m_moveProperty->m_walkableAreas.remove(id);
    }

    std::list<std::string> Object::getWalkableAreas() const
    {
        if (m_moveProperty != nullptr) {
            return m_moveProperty->m_walkableAreas;
        }
        if (m_inherited != nullptr) {
            return m_inherited->getWalkableAreas();
        }
        return {};
    }

    bool Object::operator==(Object const & obj) const
    {
        return m_name == obj.getName() && m_namespace == obj.getNamespace();
    }

    bool Object::operator!=(Object const & obj) const
    {
        return m_name != obj.getName() || m_namespace != obj.getNamespace();
    }

} // namespace FIFE
