// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "route.h"

// Standard C++ library includes
#include <cassert>
#include <iterator>
#include <limits>
#include <list>
#include <string>
#include <utility>
#include <vector>

// FIFE includes
#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/object.h"
#include "model/structures/layer.h"
#include "model/structures/location.h"
#include "util/log/logger.h"

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_STRUCTURES);
            return log;
        }
    } // namespace

    Route::Route(Location const & start, Location const & end) :
        m_status(ROUTE_CREATED),
        m_startNode(start),
        m_endNode(end),
        m_walked(0),
        m_sessionId(-1),
        m_rotation(0),
        m_replanned(false),
        m_ignoresBlocker(false),

        m_object(nullptr)
    {
    }

    Route::~Route() = default;

    void Route::setRouteStatus(RouteStatusInfo status)
    {
        m_status = status;
    }

    RouteStatusInfo Route::getRouteStatus() const
    {
        return m_status;
    }

    void Route::setStartNode(Location const & node)
    {
        m_startNode = node;
        if (m_status != ROUTE_CREATED) {
            m_status = ROUTE_CREATED;
            if (!m_path.empty()) {
                m_path.clear();
            }
            m_walked = 1;
        }
    }

    Location const & Route::getStartNode()
    {
        return m_startNode;
    }

    void Route::setEndNode(Location const & node)
    {
        if (m_status != ROUTE_CREATED) {
            m_status = ROUTE_CREATED;
            if (!m_path.empty()) {
                m_startNode = *m_current;
                m_path.clear();
            }
            m_walked = 1;
        }
        m_endNode = node;
    }

    Location const & Route::getEndNode()
    {
        return m_endNode;
    }

    Location const & Route::getCurrentNode()
    {
        if (m_path.empty()) {
            return m_startNode;
        }
        if (m_current == m_path.end()) {
            return m_path.back();
        }
        return *m_current;
    }

    Location const & Route::getPreviousNode()
    {
        if (m_path.empty()) {
            return m_startNode;
        }
        if (m_current != m_path.begin()) {
            --m_current;
            Location const & loc = *m_current;
            ++m_current;
            return loc;
        }
        return *m_current;
    }

    Location const & Route::getNextNode()
    {
        if (m_path.empty()) {
            return m_startNode;
        }
        if (m_current != m_path.end()) {
            ++m_current;
            if (m_current != m_path.end()) {
                Location const & loc = *m_current;
                --m_current;
                return loc;
            }
            --m_current;
        }
        return *m_current;
    }

    bool Route::walkToNextNode(int32_t step)
    {
        if (m_path.empty() || step == 0) {
            return false;
        }

        int64_t const pos = static_cast<int64_t>(m_walked) + static_cast<int64_t>(step);
        if (pos < 0 || std::cmp_greater_equal(pos, m_path.size())) {
            return false;
        }

        std::advance(m_current, step);
        m_walked = static_cast<uint32_t>(pos);

        return true;
    }

    bool Route::reachedEnd()
    {
        if (m_path.empty()) {
            return true;
        }
        return m_current == m_path.end();
    }

    void Route::setPath(Path const & path)
    {
        m_path = path;
        if (!m_path.empty()) {
            m_status    = ROUTE_SOLVED;
            m_current   = m_path.begin();
            m_startNode = m_path.front();
            m_endNode   = m_path.back();
        }
        m_replanned = false;
        m_walked    = 1;
    }

    Path const & Route::getPath()
    {
        return m_path;
    }

    void Route::cutPath(uint32_t length)
    {
        if (length == 0) {
            if (!m_path.empty()) {
                m_startNode = *m_current;
                m_endNode   = *m_current;
                m_path.clear();
                m_current = m_path.end();
            }
            m_status    = ROUTE_CREATED;
            m_walked    = 1;
            m_replanned = true;
            return;
        }
        if (!std::cmp_less(length, m_path.size())) {
            return;
        }

        uint32_t const newend = m_walked + length - 1;
        if (std::cmp_greater(newend, m_path.size())) {
            return;
        }

        m_path.resize(newend);
        m_endNode   = m_path.back();
        m_replanned = true;
    }

    void Route::setReplanned(bool replanned)
    {
        m_replanned = replanned;
    }

    bool Route::isReplanned() const
    {
        return m_replanned;
    }

    uint32_t Route::getPathLength()
    {
        assert(std::cmp_less_equal(m_path.size(), std::numeric_limits<uint32_t>::max()));
        return static_cast<uint32_t>(m_path.size());
    }

    uint32_t Route::getWalkedLength() const
    {
        return m_walked;
    }

    void Route::setSessionId(int32_t id)
    {
        m_sessionId = id;
    }

    int32_t Route::getSessionId() const
    {
        return m_sessionId;
    }

    void Route::setRotation(int32_t rotation)
    {
        m_rotation = rotation;
    }

    int32_t Route::getRotation() const
    {
        return m_rotation;
    }

    void Route::setCostId(std::string const & cost)
    {
        m_costId = cost;
    }

    std::string const & Route::getCostId()
    {
        return m_costId;
    }

    bool Route::isMultiCell()
    {
        if (m_object != nullptr) {
            return m_object->isMultiObject();
        }
        return false;
    }

    void Route::setOccupiedArea(std::vector<ModelCoordinate> const & area)
    {
        m_area = area;
    }

    std::vector<ModelCoordinate> const & Route::getOccupiedArea()
    {
        return m_area;
    }

    std::vector<ModelCoordinate> Route::getOccupiedCells(int32_t rotation)
    {
        if (m_object != nullptr) {
            return m_object->getMultiObjectCoordinates(rotation);
        }
        std::vector<ModelCoordinate> coords;
        return coords;
    }

    int32_t Route::getZStepRange()
    {
        if (m_object == nullptr) {
            return -1;
        }
        return m_object->getZStepRange();
    }

    bool Route::isAreaLimited()
    {
        if (m_object != nullptr) {
            if (!m_object->getWalkableAreas().empty()) {
                return true;
            }
        }
        return false;
    }

    std::list<std::string> Route::getLimitedAreas()
    {
        std::list<std::string> areas;
        if (m_object != nullptr) {
            areas = m_object->getWalkableAreas();
        }
        return areas;
    }

    void Route::setDynamicBlockerIgnored(bool ignore)
    {
        m_ignoresBlocker = ignore;
    }

    bool Route::isDynamicBlockerIgnored() const
    {
        return m_ignoresBlocker;
    }

    Path Route::getBlockingPathLocations()
    {
        Path p;
        if (!m_path.empty()) {
            // Check each path cell for blocking instances
            for (auto& it : m_path) {
                Layer* layer = it.getLayer();
                if (layer != nullptr && layer->cellContainsBlockingInstance(it.getLayerCoordinates())) {
                    p.push_back(it);
                }
            }
            // For multi-cell, also check if any footprint cells at each path position are blocked
            if (m_object != nullptr && m_object->isMultiObject()) {
                for (auto& it : m_path) {
                    Layer* layer = it.getLayer();
                    if (layer == nullptr) {
                        continue;
                    }
                    CellGrid* grid = layer->getCellGrid();
                    if (grid == nullptr) {
                        continue;
                    }
                    std::vector<ModelCoordinate> const footprint =
                        grid->toMultiCoordinates(it.getLayerCoordinates(), getOccupiedCells(m_rotation));
                    for (auto& fc : footprint) {
                        if (layer->cellContainsBlockingInstance(fc)) {
                            // Check if this blocker is part of the multi-cell's own path cells
                            bool isSelf = false;
                            for (auto& pathCell : m_path) {
                                if (pathCell.getLayerCoordinates() == fc && pathCell.getLayer() == layer) {
                                    isSelf = true;
                                    break;
                                }
                            }
                            if (!isSelf) {
                                p.push_back(it);
                                break;
                            }
                        }
                    }
                }
            }
        }
        return p;
    }

    void Route::setObject(Object* obj)
    {
        m_object = obj;
    }

    Object* Route::getObject()
    {
        return m_object;
    }

    double Route::getTotalCost() const
    {
        if (m_path.empty()) {
            return 0.0;
        }
        double cost           = 0.0;
        Location const * prev = nullptr;
        for (auto const & loc : m_path) {
            Layer* layer = loc.getLayer();
            if (layer == nullptr) {
                continue;
            }
            CellCache const * cache = layer->getCellCache();
            CellGrid* grid          = layer->getCellGrid();
            if (cache == nullptr || grid == nullptr) {
                continue;
            }
            if (prev != nullptr && prev->getLayer() == layer) {
                cost += grid->getAdjacentCost(loc.getLayerCoordinates(), prev->getLayerCoordinates());
            }
            prev = &loc;
        }
        return cost;
    }

    double Route::getRemainingCostFrom(Location const & pos) const
    {
        if (m_path.empty()) {
            return 0.0;
        }
        double cost           = 0.0;
        bool found            = false;
        Location const * prev = nullptr;
        for (auto const & loc : m_path) {
            if (!found && loc.getLayerCoordinates() == pos.getLayerCoordinates() && loc.getLayer() == pos.getLayer()) {
                found = true;
            }
            if (!found) {
                prev = &loc;
                continue;
            }
            Layer const * layer = loc.getLayer();
            if (layer == nullptr) {
                continue;
            }
            CellGrid* grid = layer->getCellGrid();
            if (grid == nullptr) {
                continue;
            }
            if (prev != nullptr && prev->getLayer() == layer) {
                cost += grid->getAdjacentCost(loc.getLayerCoordinates(), prev->getLayerCoordinates());
            }
            prev = &loc;
        }
        return cost;
    }

    bool Route::replacePathKeepingProgress(Path const & newPath, Location const & currentPos)
    {
        if (newPath.empty()) {
            return false;
        }
        // Find the cell in the new path closest to currentPos
        auto bestIt     = newPath.begin();
        double bestDist = std::numeric_limits<double>::max();
        for (auto it = newPath.begin(); it != newPath.end(); ++it) {
            if (it->getLayer() != currentPos.getLayer()) {
                continue;
            }
            ModelCoordinate const mc = it->getLayerCoordinates();
            ModelCoordinate const pc = currentPos.getLayerCoordinates();
            double const dx          = static_cast<double>(mc.x - pc.x);
            double const dy          = static_cast<double>(mc.y - pc.y);
            double const dist        = (dx * dx) + (dy * dy);
            if (dist < bestDist) {
                bestDist = dist;
                bestIt   = it;
            }
        }
        // Accept only if within 3 cells of current position
        if (bestDist > 9.0) {
            return false;
        }
        // Walk the path iterator to the new current position
        m_path    = newPath;
        m_current = m_path.begin();
        m_walked  = 1;
        // Advance to the best position
        for (auto it = m_path.begin(); it != bestIt; ++it, ++m_walked) {
            ++m_current;
        }
        m_startNode = m_path.front();
        m_endNode   = m_path.back();
        m_status    = ROUTE_SOLVED;
        m_replanned = true;
        return true;
    }

} // namespace FIFE
