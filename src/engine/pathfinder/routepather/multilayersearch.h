// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PATHFINDER_MULTILAYERSEARCH
#define FIFE_PATHFINDER_MULTILAYERSEARCH

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <list>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "util/structures/priorityqueue.h"

#include "routepathersearch.h"

namespace FIFE
{

    class CellCache;
    class Route;
    class Zone;

    /** MultiLayerSearch using A*
     */
    class FIFE_API MultiLayerSearch : public RoutePatherSearch
    {
    public:
        /** Constructor
         *
         * @param route A pointer to the route for which a path should be searched.
         * @param sessionId A integer containing the session id for this search.
         */
        MultiLayerSearch(Route* route, int32_t sessionId);

        /** Destructor
         */
        ~MultiLayerSearch() override;

        /** Updates the search.
         *
         * Each update checks all neighbors of the last checked coordinate and selects the most favorable.
         */
        void updateSearch() override;

        /** Calculates final path.
         *
         * If the search is successful then a path is created.
         */
        void calcPath() override;

    private:
        /** Creates or resets the SearchFrontier.
         *
         * @param startInt The start coordinate as integer identifier.
         * @param cache A pointer to the start CellCache.
         */
        void createSearchFrontier(int32_t startInt, CellCache* cache);

        /** Calculates path parts per layer.
         *
         */
        void calcPathStep();

        /** Fetch targets from neighbor layers.
         *
         */
        void searchBetweenTargetsNeighbor();

        /** Fetch targets from map.
         *
         */
        void searchBetweenTargetsMap();

        //! A location object representing where the search started.
        Location m_to;
        //! A location object representing where the search ended.
        Location m_from;

        //! A pointer to the start CellCache.
        CellCache* m_startCache;
        //! A pointer to the end CellCache.
        CellCache* m_endCache;
        //! A pointer to the currently used CellCache.
        CellCache* m_currentCache;

        // A pointer to the start Zone.
        Zone* m_startZone;
        // A pointer to the end Zone.
        Zone* m_endZone;

        //! The start coordinate as an int32_t.
        int32_t m_startCoordInt;
        //! The last used start coordinate as an int32_t.
        int32_t m_lastStartCoordInt;
        //! The destination coordinate as an int32_t.
        int32_t m_destCoordInt;
        //! The last used destination coordinate as an int32_t.
        int32_t m_lastDestCoordInt;
        //! The next coordinate to check out.
        int32_t m_next;

        //! The shortest path tree.
        std::vector<int32_t> m_spt;
        //! The search frontier.
        std::vector<int32_t> m_sf;
        //! A table to hold the costs.
        std::vector<double> m_gCosts;
        //! Priority queue to hold nodes on the sf in order.
        PriorityQueue<int32_t, double> m_sortedFrontier;

        //! List of targets that need to be solved to reach the real target.
        std::list<Cell*> m_betweenTargets;
        //! Indicates if last between target could be achieved
        bool m_foundLast;
        //! Path to which all steps are added.
        Path m_path;
    };
} // namespace FIFE
#endif
