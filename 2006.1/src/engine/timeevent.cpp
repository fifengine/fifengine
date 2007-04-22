/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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

#include "timeevent.h"

namespace FIFE {

	TimeEvent::TimeEvent(int period) {
		m_period		= period;
		m_last_updated	= 0;
	}

	TimeEvent::~TimeEvent() {
		return;
	}

	void TimeEvent::managerUpdateEvent(unsigned long time) {
		int time_delta = static_cast<int>(time - m_last_updated);
		if (m_period < 0) {
			return;
		} else if (m_period == 0 || time_delta >= m_period) {
			updateEvent(time);
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

	
} //FIFE
