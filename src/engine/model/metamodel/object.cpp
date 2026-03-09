// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"

#include "action.h"
#include "ipather.h"
#include "object.h"

namespace FIFE
{

    Object::BasicObjectProperty::BasicObjectProperty() :
        m_actions(nullptr), m_defaultAction(nullptr), m_blocking(false), m_static(false), m_cellStack(0)
    {
    }
    Object::BasicObjectProperty::~BasicObjectProperty()
    {
        if (m_actions != nullptr) {
            std::map<std::string, Action*>::const_iterator i(m_actions->begin());
            while (i != m_actions->end()) {
                delete i->second;
                ++i;
            }
            delete m_actions;
        }
    }

    Object::MovableObjectProperty::MovableObjectProperty() : m_pather(nullptr), m_cost(1.0), m_speed(1.0), m_zRange(0)
    {
    }
    Object::MovableObjectProperty::~MovableObjectProperty() = default;

    Object::MultiObjectProperty::MultiObjectProperty() : m_multiPart(false), m_restrictedRotation(false) { }
    Object::MultiObjectProperty::~MultiObjectProperty() = default;

    Object::Object(std::string  identifier, std::string  name_space, Object* inherited) :
        m_id(std::move(identifier)),
        m_namespace(std::move(name_space)),

        m_inherited(inherited),
        m_visual(nullptr),
        m_basicProperty(nullptr),
        m_moveProperty(nullptr),
        m_multiProperty(nullptr)
    {
    }

    Object::~Object()
    {
        delete m_visual;
        delete m_basicProperty;
        delete m_moveProperty;
        delete m_multiProperty;
    }

    Action* Object::createAction(const std::string& identifier, bool is_default)
    {
        std::map<std::string, Action*>* actions;
        if (m_basicProperty == nullptr) {
            m_basicProperty = new BasicObjectProperty();
        }

        if (m_basicProperty->m_actions == nullptr) {
            m_basicProperty->m_actions = new std::map<std::string, Action*>;
        }
        actions = m_basicProperty->m_actions;

        auto it = actions->begin();
        for (; it != actions->end(); ++it) {
            if (identifier == it->second->getId()) {
                throw NameClash(identifier);
            }
        }

        Action* a = getAction(identifier, false);
        if (a == nullptr) {
            a                      = new Action(identifier);
            (*actions)[identifier] = a;
            if (is_default || (m_basicProperty->m_defaultAction == nullptr)) {
                m_basicProperty->m_defaultAction = a;
            }
        }
        return a;
    }

    Action* Object::getAction(const std::string& identifier, bool deepsearch) const
    {
        std::map<std::string, Action*>* actions = nullptr;
        if (m_basicProperty != nullptr) {
            actions = m_basicProperty->m_actions;
        }

        std::map<std::string, Action*>::const_iterator i;
        if (actions != nullptr) {
            i = actions->find(identifier);
        }
        if ((actions == nullptr) || (i == actions->end())) {
            if ((m_inherited != nullptr) && deepsearch) {
                return m_inherited->getAction(identifier);
            }
            return nullptr;
        }
        return i->second;
    }

    std::list<std::string> Object::getActionIds() const
    {
        std::map<std::string, Action*>* actions = nullptr;
        if (m_basicProperty != nullptr) {
            actions = m_basicProperty->m_actions;
        }
        std::list<std::string> action_ids;
        if (actions != nullptr) {
            auto actions_it = actions->begin();
            for (; actions_it != actions->end(); ++actions_it) {
                action_ids.push_back(actions_it->first);
            }
        }
        return action_ids;
    }

    void Object::setDefaultAction(const std::string& identifier)
    {
        std::map<std::string, Action*>::const_iterator i;
        Action* action                          = nullptr;
        std::map<std::string, Action*>* actions = nullptr;
        if (m_basicProperty != nullptr) {
            actions = m_basicProperty->m_actions;
        }
        if (actions != nullptr) {
            i = actions->find(identifier);
        }
        if ((actions == nullptr) || (i == actions->end())) {
            if (m_inherited != nullptr) {
                action = m_inherited->getAction(identifier);
            }
        } else {
            action = i->second;
        }

        if ((action != nullptr) && (m_basicProperty != nullptr)) {
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
            m_moveProperty = new MovableObjectProperty();
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

    void Object::adoptVisual(IVisual* visual)
    {
        if ((m_visual != nullptr) && m_visual != visual) {
            delete m_visual;
        }
        m_visual = visual;
    }

    void Object::setBlocking(bool blocking)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = new BasicObjectProperty();
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
            m_basicProperty = new BasicObjectProperty();
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

    void Object::setFilename(const std::string& file)
    {
        m_filename = file;
    }

    const std::string& Object::getFilename() const
    {
        return m_filename;
    }

    void Object::setCellStackPosition(uint8_t position)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = new BasicObjectProperty();
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

    void Object::setCostId(const std::string& cost)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = new MovableObjectProperty();
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
            m_moveProperty = new MovableObjectProperty();
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
            m_moveProperty = new MovableObjectProperty();
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

    void Object::addMultiPartId(const std::string& partId)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = new MultiObjectProperty();
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

    void Object::removeMultiPartId(const std::string& partId)
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
            m_multiProperty = new MultiObjectProperty();
        }
        m_multiProperty->m_multiPart = part;
    }

    void Object::addMultiPart(Object* obj)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = new MultiObjectProperty();
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

    void Object::addMultiPartCoordinate(int32_t rotation, ModelCoordinate coord)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = new MultiObjectProperty();
        }
        m_multiProperty->m_multiPartCoordinates.insert(std::pair<int32_t, ModelCoordinate>(rotation, coord));
        m_multiProperty->m_partAngleMap[rotation] = rotation;
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
                std::multimap<int32_t, ModelCoordinate>::iterator>
                result = m_multiProperty->m_multiPartCoordinates.equal_range(closest);
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
                    const std::multimap<int32_t, ModelCoordinate>& subcoords = (*subit)->getMultiPartCoordinates();
                    m_multiProperty->m_multiObjectCoordinates.insert(subcoords.begin(), subcoords.end());
                }
                auto it =
                    m_multiProperty->m_multiObjectCoordinates.begin();
                for (; it != m_multiProperty->m_multiObjectCoordinates.end(); ++it) {
                    m_multiProperty->m_multiAngleMap[(*it).first] = (*it).first;
                }
            }
            int32_t closest = 0;
            getIndexByAngle(rotation, m_multiProperty->m_multiAngleMap, closest);
            std::pair<
                std::multimap<int32_t, ModelCoordinate>::iterator,
                std::multimap<int32_t, ModelCoordinate>::iterator>
                result = m_multiProperty->m_multiObjectCoordinates.equal_range(closest);
            auto it = result.first;
            ModelCoordinate parent(0, 0);
            coordinates.push_back(parent);
            for (; it != result.second; ++it) {
                coordinates.push_back((*it).second);
            }
        } else if (m_inherited != nullptr) {
            return m_inherited->getMultiObjectCoordinates(rotation);
        }
        return coordinates;
    }

    void Object::setRotationAnchor(const ExactModelCoordinate& anchor)
    {
        if (m_multiProperty == nullptr) {
            m_multiProperty = new MultiObjectProperty();
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
            m_multiProperty = new MultiObjectProperty();
        }
        m_multiProperty->m_restrictedRotation = restrict;
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
            m_moveProperty = new MovableObjectProperty();
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

    void Object::setArea(const std::string& id)
    {
        if (m_basicProperty == nullptr) {
            m_basicProperty = new BasicObjectProperty();
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

    void Object::addWalkableArea(const std::string& id)
    {
        if (m_moveProperty == nullptr) {
            m_moveProperty = new MovableObjectProperty();
        }
        m_moveProperty->m_walkableAreas.push_back(id);
        m_moveProperty->m_walkableAreas.sort();
        m_moveProperty->m_walkableAreas.unique();
    }

    void Object::removeWalkableArea(const std::string& id)
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

    bool Object::operator==(const Object& obj) const
    {
        return m_id == obj.getId() && m_namespace == obj.getNamespace();
    }

    bool Object::operator!=(const Object& obj) const
    {
        return m_id != obj.getId() || m_namespace != obj.getNamespace();
    }

} // namespace FIFE
