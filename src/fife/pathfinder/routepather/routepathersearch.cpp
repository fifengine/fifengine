// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "routepathersearch.h"

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
#include <vector>

#include "model/metamodel/grids/cellgrid.h"
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
            Location const loc                  = route->getStartNode();
            std::vector<ModelCoordinate> coords = route->getOccupiedArea();
            auto co_it                          = coords.begin();
            for (; co_it != coords.end(); ++co_it) {
                Cell* cell = loc.getLayer()->getCellCache()->getCell(*co_it);
                if (cell != nullptr) {
                    m_ignoredBlockers.push_back(cell);
                }
            }
        }
    }

    RoutePatherSearch::~RoutePatherSearch() = default;

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
