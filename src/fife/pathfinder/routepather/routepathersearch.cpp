// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "routepathersearch.h"

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
#include <unordered_set>
#include <vector>

#include "model/metamodel/grids/cellgrid.h"
#include "model/metamodel/object.h"
#include "model/structures/cell.h"
#include "model/structures/cellcache.h"
#include "model/structures/layer.h"
#include "pathfinder/route.h"
#include "util/math/fife_math.h"

namespace FIFE
{
    RoutePatherSearch::RoutePatherSearch(Route* route, int32_t const sessionId) :
        m_route(route),
        m_specialCost(!route->getCostId().empty()),
        m_multicell(route->isMultiCell()),
        m_ignoreDynamicBlockers(route->isDynamicBlockerIgnored()),
        m_sessionId(sessionId),
        m_status(search_status_incomplete)
    {
        m_route->setRouteStatus(ROUTE_SEARCHING);
        if (m_multicell) {
            // Store relative footprint offsets instead of raw Cell* pointers
            // This allows the search to recompute absolute cells dynamically
            // if the multi-cell rotates during the search.
            Object* obj = route->getObject();
            if (obj != nullptr) {
                m_footprintOffsets = obj->getCachedFootprint(route->getRotation());
            }
            if (m_footprintOffsets.empty()) {
                // Fallback: compute from occupied area
                Location const loc                  = route->getStartNode();
                CellGrid* grid                      = loc.getLayer()->getCellGrid();
                std::vector<ModelCoordinate> coords = route->getOccupiedArea();
                for (auto& coord : coords) {
                    m_footprintOffsets.push_back(ModelCoordinate(
                        coord.x - loc.getLayerCoordinates().x,
                        coord.y - loc.getLayerCoordinates().y,
                        coord.z - loc.getLayerCoordinates().z));
                }
            }
            assert("footprint offsets must not be empty for multi-cell search" && !m_footprintOffsets.empty());
        }
    }

    RoutePatherSearch::~RoutePatherSearch() = default;

    bool RoutePatherSearch::isIgnoredBlocker(Cell* cell, Location const & currentLoc, int32_t currentRot)
    {
        if (!m_multicell || cell == nullptr) {
            return false;
        }
        // Lazy recompute: only rebuild absolute cache when position or rotation changes
        if (!m_absoluteCache || m_lastCacheLoc.getLayerCoordinates() != currentLoc.getLayerCoordinates() ||
            m_lastCacheLoc.getLayer() != currentLoc.getLayer() || m_lastCacheRotation != currentRot) {
            m_absoluteCache.emplace();
            CellGrid* grid = currentLoc.getLayer()->getCellGrid();
            if (grid == nullptr) {
                return false;
            }
            for (auto const & offset : m_footprintOffsets) {
                std::vector<ModelCoordinate> single;
                single.push_back(offset);
                std::vector<ModelCoordinate> absVec =
                    grid->toMultiCoordinates(currentLoc.getLayerCoordinates(), single);
                if (!absVec.empty()) {
                    Cell* absCell = currentLoc.getLayer()->getCellCache()->getCell(absVec.front());
                    if (absCell != nullptr) {
                        m_absoluteCache->insert(absCell);
                    }
                }
            }
            m_lastCacheLoc      = currentLoc;
            m_lastCacheRotation = currentRot;
        }
        return m_absoluteCache->contains(cell);
    }

    int32_t RoutePatherSearch::getSessionId() const
    {
        return m_sessionId;
    }

    int32_t RoutePatherSearch::getSearchStatus() const
    {
        return m_status;
    }

    Route* RoutePatherSearch::getRoute()
    {
        return m_route;
    }

    void RoutePatherSearch::setSearchStatus(SearchStatus const status)
    {
        m_status = status;
    }
} // namespace FIFE
