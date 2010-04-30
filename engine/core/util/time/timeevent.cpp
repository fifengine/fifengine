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
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "timemanager.h"
#include "timeevent.h"

namespace FIFE {

	TimeEvent::TimeEvent(int period):
		m_period(period),
		m_last_updated(TimeManager::instance()->getTime()) {
	}

	TimeEvent::~TimeEvent() {
		return;
	}

	void TimeEvent::managerUpdateEvent(unsigned long time) {
		int time_delta = static_cast<int>(time - m_last_updated);
		if (m_period < 0) {
			return;
		} else if (m_period == 0 || time_delta >= m_period) {
			updateEvent(time_delta);
			m_last_updated = time;
		}
	}

	void TimeEvent::setPeriod(int period) {
		m_period = period;
	}

	int TimeEvent::getPeriod() {
		return m_period;
	}

	unsigned long TimeEvent::getLastUpdateTime() {
		return m_last_updated;
	}

	void TimeEvent::setLastUpdateTime(unsigned long ms) {
		m_last_updated = ms;
	}


} //FIFE
