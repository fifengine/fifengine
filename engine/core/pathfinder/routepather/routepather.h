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

#ifndef FIFE_PATHFINDER_ROUTEPATHER
#define FIFE_PATHFINDER_ROUTEPATHER

// Standard C++ library includes
#include <map>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/abstractpather.h"
#include "model/structures/location.h"
#include "model/structures/map.h"
#include "util/structures/priorityqueue.h"

namespace FIFE {

	class Search;
	class SearchSpace;
	class RoutePatherSearch;

	class RoutePather : public AbstractPather {
	public:
		/** Constructor.
		 *
		 */
		RoutePather() : m_map(0), m_nextFreeSessionId(0), m_maxticks(1000) {
		}
		
		void setMap(Map* map);
		int getNextLocation(const Instance* instance, const Location& target, 
		                    double distance_to_travel, Location& nextLocation,
		                    Location& facingLocation, int session_id=-1, 
							int priority = MEDIUM_PRIORITY);

		/** Updates the route pather.
		 *
		 * Advances the active search by so many time steps. If the search
		 * completes then this function pops it from the active session list and
		 * continues updating the next session until it runs out of time.
		 */
		void update();

		/** Cancels a session.
		 *
		 * Cancels a route pather session. Determines if it is an active session
		 * or not and acts accordingly. 
		 *
		 * @param session_id The id of the session to cancel.
		 * @return True if the session could be canceled false otherwise.
		 */
		bool cancelSession(const int session_id);

		/** Adds a search space to the route pather.
		 *
		 * @param search_space A pointer to the search space to add.
		 * @return a boolean signifying whether the search space was correctly added or not.
		 */
		bool addSearchSpace(SearchSpace* search_space);

		/** Retrieves a searchspace associated with the given layer.
		 *
		 * @param A pointer to the search space
		 * @return A pointer to the search space if it could be found, 0 otherwise.
		 */
		SearchSpace* getSearchSpace(Layer * const layer);

		std::string getName() const { return "RoutePather"; };		
	private:
		typedef std::list<Location> Path;
		typedef PriorityQueue<Search*, int> SessionQueue;
		typedef std::list<int> SessionList;
		typedef std::map<int, Path> PathMap;
		typedef std::map<Layer*, SearchSpace*> SearchSpaceMap;
		/** Makes the instance follow the given path.
		 *
		 * Calculates the next position the instance should move to given the 
		 * the instance's speed. 
		 *
		 * @param instance A pointer to the instance to move.
		 * @param speed The speed to move the instance.
		 * @param nextLocation An out variable which will store the instances next location.
		 * @param facingLocation An out variable which will store the instances facing location.
		 */
		void followPath(const Instance* instance, Path& path, double speed, Location& nextLocation, Location& facingLocation);

		/** Adds a session id to the session map.
		 *
		 * Stores the given session id in the session map.
		 *
		 * @param sessionId The session id to store.
		 */
		void addSessionId(const int sessionId);


		/** Determines if the given session Id is valid.
		 *
		 * Searches the session list to determine if a search with the given session id
		 * has been registered.
		 * 
		 * @return true if one has, false otherwise.
		 */
		bool sessionIdValid(const int sessionId);

		/** Removes a session id from the session map.
		 *
		 * @param sessionId The session id to remove.
		 * @return True if the sessionId could be removed, false otherwise.
		 */
		bool invalidateSessionId(const int sessionId);

		//The map the search is running on.
		Map*	       m_map;

		//A map of currently running sessions (searches).
		SessionQueue   m_sessions;

		//A list of session ids that have been registered.
		SessionList    m_registeredSessionIds;

		//Calculated paths for the movement phase.
		PathMap		   m_paths;

		//A map of searchspaces.
		SearchSpaceMap m_searchspaces; 

		//The next free session id.
		int            m_nextFreeSessionId;

		//The maximum number of ticks allowed.
		int			   m_maxticks;
	};

}

#endif
