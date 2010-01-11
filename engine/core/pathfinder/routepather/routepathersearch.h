/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_PATHFINDER_ROUTEPATHERSEARCH
#define FIFE_PATHFINDER_ROUTEPATHERSEARCH

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/priorityqueue.h"

namespace FIFE {

	class Map;
	class SearchSpace;
	class Heuristic;

	/** RoutePatherSearch using A*
	 *
	 * For now this class uses offline A*, however eventually this will be switched over to RTA*.
	 */
	class RoutePatherSearch {
	public:
		RoutePatherSearch(const int session_id, const Location& from, const Location& to, SearchSpace* searchSpace);

                typedef std::list<Location> Path;
                /** An enumeration of the different status the search can be in.
                 *
                 */
                enum SearchStatus {
                        search_status_failed,
                        search_status_complete,
                        search_status_incomplete
                };

		virtual void updateSearch();

		virtual Path calcPath();

                /** Retrieves the session id.
                 *
                 * @return The searches session id in the pather.
                 */
                int getSessionId() const {
                        return m_sessionId;
                }

                /** Retrieves the pather.
                 *
                 * @return A pointer to the abstract pather which
                 */
                SearchSpace* getSearchSpace() const {
                        return m_searchspace;
                }

                /** A small function which returns the current status of the search.
                 *
                 * @return An integer value representing the status, which is enumerated by this class.
                 */
                int getSearchStatus() const {
                        return m_status;
                }

         protected:
                /** Sets the current status of the search.
                 *
                 * @param status The status to set.
                 */
                void setSearchStatus(const SearchStatus status) {
                        m_status = status;
                }

         private:
                //A location object representing where the search started.
                Location                m_to;

                //A location object representing where the search ended.
                Location                m_from;

                //An integer containing the session id for this search.
                int                             m_sessionId;

                //A pointer to the pather that owns this search.
                SearchSpace*    m_searchspace;

                //An enumeration of the searches current status.
                SearchStatus    m_status;
                
                //The start coordinate as an int.
                int             m_startCoordInt;
                
                //The destination coordinate as an int.
                int             m_destCoordInt;
                
                //The next coordinate to check out.
                int             m_next;

                //The class to use to calculate the heuristic value.
                Heuristic*                m_heuristic;

		//The shortest path tree.
		std::vector<int>          m_spt;

		//The search frontier.
		std::vector<int>	      m_sf;

		//A table to hold the costs.
		std::vector<float>		  m_gCosts;

		//priority queue to hold nodes on the sf in order. 
		PriorityQueue<int, float> m_sortedfrontier;
	};
}
#endif
