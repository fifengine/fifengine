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

#ifndef FIFE_MODEL_ABSTRACTPATHER_H
#define FIFE_MODEL_ABSTRACTPATHER_H

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Map;
	class Location;
	class Instance;

	enum 
	{
		HIGH_PRIORITY,
		MEDIUM_PRIORITY,
		LOW_PRIORITY
	};

	class AbstractPather {
	public:
		virtual ~AbstractPather() {};

		/** Gets next location from pathfinder
		 * Model will call this function periodically when instances should
		 * move on map. Pather must return next location where instance should be moved.
		 * Pather must be able to store multiple sessions for multiple simultaneous searches
		 * (coming from multiple instances)
		 *
		 * @param instance Instance making the call
		 * @param target Target of the movement. This must be always on same layer as curpos
		 * @param distance_to_travel  Distance how far caller can travel (in layer units)
		 * @param nextLocation next location returned by the pather
		 * @param facingLocation next facing location returned by the pather
		 * @param session_id pather can do pathfinding in increments.
		 *   Further increments are bind to previous ones with given session_id
		 * @param priority The priority to assign to search (high are pushed to the front of the queue).
		 * @return session_id to use with further calls
		 */
		virtual int getNextLocation(const Instance* instance, const Location& target, 
		                            double distance_to_travel, Location& nextLocation,
		                            Location& facingLocation, int session_id=-1, 
									int priority = MEDIUM_PRIORITY) = 0;

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
		 * @param session_id The id of the session to cancel.
		 * @return A boolean to signify whether the session was successfully found and cancelled.
		 */
		virtual bool cancelSession(const int session_id) = 0;

		/** Gets the name of this pather
		 */
		virtual std::string getName() const = 0;
	};
}

#endif
