/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#ifndef FIFE_MODEL_ABSTRACTPATHER_H
#define FIFE_MODEL_ABSTRACTPATHER_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Map;
	class Location;

	class AbstractPather {
	public:
		virtual ~AbstractPather() {};

		/** Sets map for pather to use
		 */
		virtual void setMap(Map* map) = 0;

		/** Gets next locations from pathfinder
		 * Model will call this function periodically when instances are
		 * moving on map. Pather must return next locations in 
		 * such pace that model always knows where to instances should be moved.
		 * Pather must be able to store multiple chains for multiple simultaneous paths
		 * (coming from multiple instances)
		 *
		 * @param curpos Current locations of the movement
		 * @param target Target of the movement. This must be always on same layer as curpos
		 * @param nextnodes locations where to move next (returned by pather). In case
		 *                  called when already at target, returns the target cell
		 * @param session_id pather does pathfinding in increments.
		 *   Further increments are bind to previous ones with given session_id
		 * @return session_id to use with further calls
		 */
		virtual int getNextLocations(const Location& curpos, const Location& target, 
		                             std::vector<Location>& nextlocations, const int session_id=-1) = 0;

		/** Cancels the given session.
		 *
		 * @param session_id The id of the session to cancel.
		 */
		virtual bool cancelSession(const int session_id) = 0;

		/**
		 *
		 */
		virtual void resetTicks() = 0;
	};
}

#endif
