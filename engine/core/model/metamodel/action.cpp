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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "util/exception.h"

#include "action.h"

namespace FIFE { namespace model {
	Action::Action() {
	}

	Action::~Action() {
	}

	int Action::getAnimationIndexByAngle(unsigned int angle) {
		int wangle = angle % 360;

		if (m_animations.size() == 0) {
			return -1;
		}
		if (m_animations.size() == 1) {
			return m_animations.begin()->second;
		}

		type_animmap::const_iterator u(m_animations.upper_bound(wangle));
		type_animmap::const_iterator tmp;

		// take care of the forward wrapping case
		if (u == m_animations.end()) {
			int ud = wangle - (--u)->first;
			int ld = 360 - wangle + m_animations.begin()->first;
			if (ud > ld) {
				// wrapped value (first)
				return m_animations.begin()->second;
			}
			// non-wrapped value
			return u->second;
		}

		// take care of the backward wrapping case
		if (u == m_animations.begin()) {
			tmp = m_animations.end();
			tmp--;
			int ld = u->first - wangle;
			int ud = 360 - tmp->first + wangle;
			if (ud > ld) {
				// non-wrapped value (first)
				return m_animations.begin()->second;
			}
			// wrapped value (last)
			return u->second;
		}

		// value in the middle...
		int ud = u->first - wangle;
		int ui = u->second;
		u--;
		int ld = wangle - u->first;
		int li = u->second;

		if (ud <= ld) {
			return ui;
		}
		return li;
	}

	void Action::addAnimation(unsigned int angle, int animation_index) {
		m_animations[angle % 360] = animation_index;
	}
}}
