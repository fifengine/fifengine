/***************************************************************************
 *   Copyright (C) 2006-2011 by the FIFE team                              *
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

#ifndef FIFE_MODEL_IPATHER_H
#define FIFE_MODEL_IPATHER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Location;
	class Instance;
	class Route;
	
	//! A path is a list with locations. Each location holds the coordinate for one cell.
	typedef std::list<Location> Path;

	enum PriorityType {
		HIGH_PRIORITY,
		MEDIUM_PRIORITY,
		LOW_PRIORITY
	};

	class IPather {
	public:
		virtual ~IPather() {};

		/** Creates a route between the start and end location that needs be solved.
		 *
		 * @param start A const reference to the start location.
		 * @param end A const reference to the target location.
		 * @param immediate A optional boolean, if true the route bypass the max. ticks limit and solves the path immediately, otherwise false.
		 * @param cost_id A const reference to the string that holds the cost identifier. You can use it optional then this cost id is used instead of the default cost.
		 */
		virtual Route* createRoute(const Location& start, const Location& end, bool immediate = false, const std::string& cost_id = "") = 0;

		/** Solves the route to create a path.
		 *
		 * @param route A pointer to the route which should be solved.
		 * @param priority The priority to assign to search (high are pushed to the front of the queue). @see PriorityType
		 * @param immediate A optional boolean, if true the route bypass the max. ticks limit and solves the path immediately, otherwise false.
		 * @return A boolean, if true the route could be solved, otherwise false.
		 */
		virtual bool solveRoute(Route* route, int32_t priority = MEDIUM_PRIORITY, bool immediate = false) = 0;

		/** Follows the path of the route.
		 *
		 * @param current A const reference to the current location.
		 * @param route A pointer to the route which should be followed.
		 * @param speed A double which holds the speed.
		 * @param nextLocation A reference to the next location returned by the pather.
		 * @return A boolean, if true the route could be followed, otherwise false.
		 */
		virtual bool followRoute(const Location& current, Route* route, double speed, Location& nextLocation) = 0;

		/** Updates the pather (should it need updating).
		 *
		 * The update method is called by the model. Pathfinders which require per loop updating
		 * (in the case of pathfinders which implement A* for instance) should use this method
		 * as an oppurtunity to update the search. Generally the method should be constrained to
		 * a maximum amount of search updating to prevent this method from stalling the application.
		 */
		virtual void update() = 0;

		/** Cancels a given session.
		 *
		 * This function is called when (for instance) the user changes their mind about
		 * a destination while the agent is already moving, the old session needs to be
		 * cancelled and a new one created.
		 *
		 * @param sessionId The id of the session to cancel.
		 * @return A boolean to signify whether the session was successfully found and cancelled.
		 */
		virtual bool cancelSession(const int32_t sessionId) = 0;

		/** Sets maximal ticks (update steps) to solve routes. @see update()
		 * @param ticks A integer which holds the steps. default is 1000
		 */
		virtual void setMaxTicks(int32_t ticks) = 0;

		/** Returns maximal ticks (update steps) to solve routes. @see update()
		 * @return A integer which holds the steps. default is 1000
		 */
		virtual int32_t getMaxTicks() = 0;

		/** Gets the name of this pather
		 */
		virtual std::string getName() const = 0;
	};
}

#endif
