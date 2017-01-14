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

#ifndef FIFE_ROUTE_H
#define FIFE_ROUTE_H

// Standard C++ library includes
#include <list>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

namespace FIFE {

	class Location;
	class Object;

	/** Defines different route status types for the search.
	 *
	 * ROUTE_CREATED means, route is created but has no path and no search.
	 * ROUTE_SEARCHING means, route is created but has no path and it is still being sought.
	 * ROUTE_SEARCHED means, route is created and the search completed, on the next step the route is solved.
	 * ROUTE_SOLVED means that a path could be found.
	 * ROUTE_FAILED means the search failed. No path could be found.
	 */
	enum RouteStatus {
		ROUTE_CREATED = 0,
		ROUTE_SEARCHING,
		ROUTE_SEARCHED,
		ROUTE_SOLVED,
		ROUTE_FAILED
	};
	typedef uint8_t RouteStatusInfo;

	//! A path is a list with locations. Each location holds the coordinate for one cell.
	typedef std::list<Location> Path;

	/** A basic route.
	 * Holds the path and all related infos.
	 */
	class Route : public FifeClass {
	public:
		/** Constructor
		 * @param start A const reference to the start location.
		 * @param end A const reference to the end/target location.
		 */
		Route(const Location& start, const Location& end);
		
		/** Destructor
		 */
		~Route();

		/** Sets route status.
		 * @param status The seach status that should be set.
		 */
		void setRouteStatus(RouteStatusInfo status);

		/** Returns route status.
		 * @return The seach status that is set.
		 */
		RouteStatusInfo getRouteStatus();

		/** Sets the start location.
		 * @param node A const reference to the start location.
		 */
		void setStartNode(const Location& node);

		/** Returns the start location.
		 * @return A const reference to the start location.
		 */
		const Location& getStartNode();

		/** Sets the target location.
		 * @param node A const reference to the end location.
		 */
		void setEndNode(const Location& node);

		/** Returns the target location.
		 * @return A const reference to the end location.
		 */
		const Location& getEndNode();

		/** Returns current location.
		 * @return A const reference to the currently used location.
		 */
		const Location& getCurrentNode();

		/** Returns previous location.
		 * @return A const reference to the previous location.
		 */
		const Location& getPreviousNode();

		/** Returns next location.
		 * @return A const reference to the next location.
		 */
		const Location& getNextNode();

		/** Changes the position on the path.
		 * Changes the results of getCurrentNode(), getPreviousNode() and getPreviousNode().
		 * @param step A integer that indicates how much steps should be walked along the path, default is one.
		 * @return A boolean, true if the position could be changed, false otherwise e.g. at path end.
		 */
		bool walkToNextNode(int32_t step = 1);

		/** Gets if the end of the path was achieved.
		 * @return A boolean, if true the end is achieved, false otherwise.
		 */
		bool reachedEnd();

		/** Sets the path for the route.
		 * @param path A const reference to the path.
		 */
		void setPath(const Path& path);

		/** Returns the path.
		 * @return The path which contains all steps.
		 */
		Path getPath();

		/** Cuts path after the given length.
		 * @param length The new length of the path.
		 */
		void cutPath(uint32_t length = 1);

		/** Sets the route to replanned.
		 * @param replanned A boolean that indicates if true the route is replanned, otherwise false.
		 */
		void setReplanned(bool replanned);

		/** Gets if the route is replanned.
		 * @return A boolean, if true the route is replanned, otherwise false.
		 */
		bool isReplanned();

		/** Returns the length of the path.
		 * @return The path length.
		 */
		uint32_t getPathLength();

		/** Returns the walked steps.
		 * @return The number of walked steps.
		 */
		uint32_t getWalkedLength();

		/** Sets the session identifier.
		 * @param id A integer as identifier for the search session.
		 */
		void setSessionId(int32_t id);

		/** Returns the session identifier.
		 * @return A integer as identifier for the search session.
		 */
		int32_t getSessionId();

		/** Sets the current rotation.
		 * @param rotation The rotation as integer.
		 */
		void setRotation(int32_t rotation);

		/** Returns the current rotation.
		 * @return The rotation as integer.
		 */
		int32_t getRotation();

		/** Sets cost identifier which should be used for pathfinding.
		 * @param cost A const reference to the string that contains the identifier.
		 */
		void setCostId(const std::string& cost);

		/** Returns cost identifier which is used for pathfinding.
		 * @return A const reference to the string that contains the identifier.
		 */
		const std::string& getCostId();

		/** Gets if path is for a multi cell object.
		 * @return A boolean, true if path is for multi cell, otherwise false.
		 */
		bool isMultiCell();

		/** Sets occupied coordinates for multi cell object.
		 * @param area A const reference to a vector that contains the coordinates.
		 */
		void setOccupiedArea(const std::vector<ModelCoordinate>& area);

		/** Returns occupied coordinates for multi cell object.
		 * @return A const reference to a vector that contains the coordinates.
		 */
		const std::vector<ModelCoordinate>& getOccupiedArea();

		/** Returns relative coordinates for multi cell object based on rotation.
		 * @param rotation The angle which is used to determine the occupied coordinates.
		 * @return A const reference to a vector that contains the coordinates.
		 */
		std::vector<ModelCoordinate> getOccupiedCells(int32_t rotation);

		/** Returns z-step range from object. In case it is not limited -1 is returned.
		 * @return The z-step range as int.
		 */
		int32_t getZStepRange();

		bool isAreaLimited();
		const std::list<std::string> getLimitedAreas();

		/** Sets the route to ignore dynamic blocker.
		 * @param ignore A boolean that indicates if true the route ignores blocker, otherwise false.
		 */
		void setDynamicBlockerIgnored(bool ignore);

		/** Gets if the route ignores dynamic blocker.
		 * @return A boolean, if true the route ignores blocker, otherwise false.
		 */
		bool isDynamicBlockerIgnored();

		/** Returns the blocking locations of the path.
		 * Only useful in case the blocking ignore flag is set.
		 * @return A location list that contains all blocking locations of the path.
		 */
		Path getBlockingPathLocations();

		/** Sets the object, needed for multi cell and z-step range.
		 * @param obj A pointer to the object.
		 */
		void setObject(Object* obj);

		/** Returns the object, needed for multi cell and z-step range.
		 * @return A pointer to the object.
		 */
		Object* getObject();

	private:
		//! path iterator
		typedef Path::iterator PathIterator;

		//! search status
		RouteStatusInfo m_status;

		//! start location
		Location m_startNode;

		//! end location
		Location m_endNode;

		//! path
		Path m_path;

		//! current position on the path
		PathIterator m_current;

		//! walked steps on the path
		uint32_t m_walked;

		//! session id of the search
		int32_t m_sessionId;

		//! current rotation
		int32_t m_rotation;

		//! is path replanned
		bool m_replanned;

		//! ignores dynamic blocker
		bool m_ignoresBlocker;

		//! used cost identifier
		std::string m_costId;

		//! occupied cells by multicell object
		std::vector<ModelCoordinate> m_area;

		//! pointer to multi object
		Object* m_object;
	};

} // FIFE

#endif
