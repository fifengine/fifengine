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

#ifndef FIFE_ANGLES_H
#define FIFE_ANGLES_H

// Standard C++ library includes
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"

namespace FIFE {
	typedef std::map<unsigned int, int> type_angle2id;

	/** Returns id for given angle from angle2id map
	 * in case there are no elements in the map, negative value is returned
	 */
	int getIndexByAngle(int angle, const type_angle2id& angle2id, int& closestMatchingAngle);

	/** Gets angle of vector defined by given locations
	 *  @return angle in polar coordinates between the line
	 *  defined by the two locations and the horizontal axis (East <-> West)
	 */
	inline int getAngleBetween(const Location& loc1, const Location& loc2) {
		ExactModelCoordinate c1 = loc1.getMapCoordinates();
		ExactModelCoordinate c2 = loc2.getMapCoordinates();

		double dy = (c2.y - c1.y);
		double dx = (c2.x - c1.x);

		int angle = static_cast<int>(Mathd::ATan2(-dy,dx)*(180.0/Mathd::pi()));

		return angle;
	}
}

#endif
