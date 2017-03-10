/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
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
#include "model/metamodel/ipather.h"
#include "model/structures/location.h"
#include "util/structures/priorityqueue.h"

namespace FIFE {

	class CellCache;
	class RoutePatherSearch;
	class Route;

	class RoutePather : public IPather {
	public:
		/** Constructor.
		 *
		 */
		RoutePather() : m_nextFreeSessionId(0), m_maxTicks(1000) {
		}

		/** Creates a route between the start and end location that needs be solved.
		 *
		 * @param start A const reference to the start location.
		 * @param end A const reference to the target location.
		 * @param immediate A optional boolean, if true the route bypass the max. ticks limit and solves the path immediately, otherwise false.
		 * @param costId A const reference to the string that holds the cost identifier. You can use it optional then this cost id is used instead of the default cost.
		 */
		Route* createRoute(const Location& start, const Location& end, bool immediate = false, const std::string& costId = "");

		/** Solves the route to create a path.
		 *
		 * @param route A pointer to the route which should be solved.
		 * @param priority The priority to assign to search (high are pushed to the front of the queue). @see PriorityType
		 * @param immediate A optional boolean, if true the route bypass the max. ticks limit and solves the path immediately, otherwise false.
		 * @return A boolean, if true the route could be solved, otherwise false.
		 */
		bool solveRoute(Route* route, int32_t priority = MEDIUM_PRIORITY, bool immediate = false);

		/** Follows the path of the route.
		 *
		 * @param current A const reference to the current location.
		 * @param route A pointer to the route which should be followed.
		 * @param speed A double which holds the speed.
		 * @param nextLocation A reference to the next location returned by the pather.
		 * @return A boolean, if true the route could be followed, otherwise false.
		 */
		bool followRoute(const Location& current, Route* route, double speed, Location& nextLocation);
		
		/** Updates the route pather.
		 *
		 * Advances the active search by so many time steps. If the search
		 * completes then this function pops it from the active session list and
		 * continues updating the next session until it runs out of time.
		 * @see setMaxTicks()
		 */
		void update();

		/** Cancels a session.
		 *
		 * Cancels a route pather session. Determines if it is an active session
		 * or not and acts accordingly.
		 *
		 * @param sessionId The id of the session to cancel.
		 * @return True if the session could be canceled false otherwise.
		 */
		bool cancelSession(const int32_t sessionId);

		/** Sets maximal ticks (update steps) to solve routes. @see update()
		 * @param ticks A integer which holds the steps. default is 1000
		 */
		void setMaxTicks(int32_t ticks);

		/** Returns maximal ticks (update steps) to solve routes. @see update()
		 * @return A integer which holds the steps. default is 1000
		 */
		int32_t getMaxTicks();

		/** Returns name of the pathfinder.
		 * @return A string that contains the name of the pathfinder.
		 */
		std::string getName() const;

	private:
		//! A path is a list with locations. Each location holds the coordinate for one cell.
		typedef std::list<Location> Path;

		//! Holds the searches and their priority.
		typedef PriorityQueue<RoutePatherSearch*, int32_t> SessionQueue;

		//! Holds the sessions.
		typedef std::list<int32_t> SessionList;

		/** Adds a session id to the session map.
		 *
		 * Stores the given session id in the session map.
		 *
		 * @param sessionId The session id to store.
		 */
		void addSessionId(const int32_t sessionId);

		/** Makes a new session id.
		 *
		 *  @return The new session id.
		 */
		int32_t makeSessionId();

		/** Are two locations equivalent from the perspective of pathing (same layer coordinates and layer).
		 *
		 * @param a A const reference to the first location to check.
		 * @param b A const reference to the second location to check.
		 * @return A boolean, true if the locations are equal, false otherwise.
		 */
		bool locationsEqual(const Location& a, const Location& b);

		/** Determines if the given session Id is valid.
		 *
		 * Searches the session list to determine if a search with the given session id
		 * has been registered.
		 * @param sessionId The session id to check.
		 * @return true if one has, false otherwise.
		 */
		bool sessionIdValid(const int32_t sessionId);

		/** Removes a session id from the session map.
		 *
		 * @param sessionId The session id to remove.
		 * @return True if the sessionId could be removed, false otherwise.
		 */
		bool invalidateSessionId(const int32_t sessionId);

		//! A map of currently running sessions (searches).
		SessionQueue m_sessions;

		//! A list of session ids that have been registered.
		SessionList m_registeredSessionIds;

		//! The next free session id.
		int32_t m_nextFreeSessionId;

		//! The maximum number of ticks allowed.
		int32_t m_maxTicks;
	};
}
#endif
