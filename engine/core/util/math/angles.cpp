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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "angles.h"

namespace FIFE {
	int32_t getIndexByAngle(int32_t angle, const type_angle2id& angle2id, int32_t& closestMatchingAngle) {
		int32_t wangle = (360 + angle) % 360;

		if (angle2id.size() == 0) {
			return -1;
		}
		if (angle2id.size() == 1) {
			closestMatchingAngle = angle2id.begin()->first;
			return angle2id.begin()->second;
		}

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

		if (ud <= ld) {
			closestMatchingAngle = ucm;
			return ui;
		}
		closestMatchingAngle = lcm;
		return li;
	}
}
