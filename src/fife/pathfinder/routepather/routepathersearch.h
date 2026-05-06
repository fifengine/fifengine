// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PATHFINDER_ROUTEPATHERSEARCH
#define FIFE_PATHFINDER_ROUTEPATHERSEARCH

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>

// 3rd party library includes

// FIFE includes
#include <vector>

#include "util/structures/priorityqueue.h"

namespace FIFE
{

    class Cell;
    class CellCache;
    class Route;

    /** RoutePatherSearch using A*
     *
     */
    class /*FIFE_API*/ RoutePatherSearch
    {
        public:
            /** Constructor
             *
             * @param route A pointer to the route for which a path should be searched.
             * @param sessionId A integer containing the session id for this search.
             */
            RoutePatherSearch(Route* route, int32_t sessionId);

            virtual ~RoutePatherSearch();

            /** An enumeration of the different status the search can be in.
             *
             */
            enum SearchStatus : std::uint8_t
            {
                search_status_failed,
                search_status_complete,
                search_status_incomplete
            };

            /** Updates the search.
             *
             * Each update checks all neighbors of the last checked coordinate and selects the most favorable.
             */
            virtual void updateSearch() = 0;

            /** Calculates final path.
             *
             * If the search is successful then a path is created.
             */
            virtual void calcPath() = 0;

            /** Retrieves the session id.
             *
             * @return The searches session id in the pather.
             */
            int32_t getSessionId() const;

            /** A small function which returns the current status of the search.
             *
             * @return An integer value representing the status, which is enumerated by this class.
             */
            int32_t getSearchStatus() const;

            /** Returns the associated route for this search.
             *
             * @return A pointer to the route.
             */
            Route* getRoute();

        protected:
            /** Sets the current status of the search.
             *
             * @param status The status to set.
             * @see SearchStatus
             */
            void setSearchStatus(SearchStatus status);

            //! Pointer to route
            Route* m_route;

            //! Indicates if the search should use special costs.
            bool m_specialCost;

            //! Indicates if the route is for a multi cell object.
            bool m_multicell;

            //! Indicates if dynamic blockers should be ignored.
            bool m_ignoreDynamicBlockers;

            //! Blockers from a multi cell object which should be ignored.
            std::vector<Cell*> m_ignoredBlockers;

        private:
            //! An integer containing the session id for this search.
            int32_t m_sessionId;

            //! An enumeration of the searches current status.
            SearchStatus m_status;
    };
} // namespace FIFE
#endif
