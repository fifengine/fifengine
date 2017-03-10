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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/metamodel/grids/cellgrid.h"
#include "model/structures/layer.h"

#include "angles.h"

namespace FIFE {
	int32_t getIndexByAngle(int32_t angle, const type_angle2id& angle2id, int32_t& closestMatchingAngle) {
		if (angle2id.empty()) {
			return -1;
		}
		if (angle2id.size() == 1) {
			closestMatchingAngle = angle2id.begin()->first;
			return angle2id.begin()->second;
		}

		int32_t wangle = (360 + angle) % 360;
		type_angle2id::const_iterator u(angle2id.upper_bound(wangle));
		type_angle2id::const_iterator tmp;

		// take care of the forward wrapping case
		if (u == angle2id.end()) {
			int32_t ud = wangle - (--u)->first;
			int32_t ld = 360 - wangle + angle2id.begin()->first;
			if (ud > ld) {
				// wrapped value (first)
				closestMatchingAngle = angle2id.begin()->first;
				return angle2id.begin()->second;
			}
			// non-wrapped value
			closestMatchingAngle = u->first;
			return u->second;
		}

		// take care of the backward wrapping case
		if (u == angle2id.begin()) {
			tmp = angle2id.end();
			tmp--;
			int32_t ld = u->first - wangle;
			int32_t ud = 360 - tmp->first + wangle;
			if (ud > ld) {
				// non-wrapped value (first)
				closestMatchingAngle = angle2id.begin()->first;
				return angle2id.begin()->second;
			}
			// wrapped value (last)
			closestMatchingAngle = tmp->first;
			return tmp->second;
		}

		// value in the middle...
		int32_t ud = u->first - wangle;
		int32_t ucm = u->first;
		int32_t ui = u->second;
		u--;
		int32_t ld = wangle - u->first;
		int32_t lcm = u->first;
		int32_t li = u->second;

		// if ud and ls is equal then lcm is prefered (next angle)
		if (ud < ld) {
			closestMatchingAngle = ucm;
			return ui;
		}
		closestMatchingAngle = lcm;
		return li;
	}

	int32_t getAngleBetween(const Location& loc1, const Location& loc2) {
		ExactModelCoordinate c1 = loc1.getMapCoordinates();
		ExactModelCoordinate c2 = loc2.getMapCoordinates();

		double dy = (c2.y - c1.y);
		double dx = (c2.x - c1.x);
		// add grid rotation to angle, to guarantee uniform angles (not grid based)
		int32_t angle = round(Mathd::ATan2(-dy,dx)*(180.0/Mathd::pi()) + loc1.getLayer()->getCellGrid()->getRotation());
		if (angle < 0) {
			angle += 360;
		}
		angle %= 360;
		return angle;
	}
	
	Location getFacing(const Location& loc, const int32_t angle) {
		Location facing(loc);
		ExactModelCoordinate emc = facing.getMapCoordinates();
		// remove grid rotation from angle, to guarantee uniform angles (not grid based)
		double tmpAngle = static_cast<double>(angle) - loc.getLayer()->getCellGrid()->getRotation();
		emc.x += Mathd::Cos(tmpAngle * (Mathd::pi()/180.0));
		emc.y -= Mathd::Sin(tmpAngle * (Mathd::pi()/180.0));
		facing.setMapCoordinates(emc);

		return facing;
	}

	int32_t getAngleBetween(const ExactModelCoordinate& emc1, const ExactModelCoordinate& emc2) {
		double dy = (emc2.y - emc1.y);
		double dx = (emc2.x - emc1.x);

		int32_t angle = round(Mathd::ATan2(-dy,dx)*(180.0/Mathd::pi()));
		if (angle < 0) {
			angle += 360;
		}
		angle %= 360;
		return angle;
	}
	
	ExactModelCoordinate getFacing(const ExactModelCoordinate& emc, const int32_t angle) {
		ExactModelCoordinate result = emc;
		result.x += Mathd::Cos(static_cast<double>(angle) * (Mathd::pi()/180.0));
		result.y -= Mathd::Sin(static_cast<double>(angle) * (Mathd::pi()/180.0));
		return result;
	}
}
