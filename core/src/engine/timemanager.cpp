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
#include <algorithm>
#include <cassert>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/debugutils.h"
#include "timeevent.h"
#include "timemanager.h"

namespace FIFE {

	TimeManager::TimeManager() {
		m_current_time	= SDL_GetTicks();
		m_time_delta	= 1;
		m_average_frame_time = 0;
	}

	TimeManager::~TimeManager() {
	}

	void TimeManager::update() {
		// Set new current time, time delta and framerate.
		m_time_delta = m_current_time;
		m_current_time = SDL_GetTicks();
		m_time_delta = m_current_time - m_time_delta;

		double alpha = .985;
		m_average_frame_time = m_average_frame_time*alpha + double(m_time_delta)*(1.-alpha);

		// Update live events.
		//
		// It is very important to NOT use iterators (over a vector)
		// here, as an event might add enough events to resize the vector.
		// -> Ugly segfault
		for (size_t i = 0; i != m_events_list.size(); ++i) {
			TimeEvent* event = m_events_list[ i ];
			if( event ) {
				event->managerUpdateEvent(m_current_time);
			}
		}

		// Remove dead events
		std::vector<TimeEvent*>::iterator it;
		it = std::remove( m_events_list.begin(), m_events_list.end(), static_cast<TimeEvent*>(0));
		m_events_list.erase( it, m_events_list.end());
	}

	void TimeManager::registerEvent(TimeEvent* event) {
		// Register.
		m_events_list.push_back(event);
	}

	void TimeManager::unregisterEvent(TimeEvent* event) {
		// Unregister.
		std::vector<TimeEvent*>::iterator i = m_events_list.begin();
		for(; i != m_events_list.end(); ++i) {
			if( (*i) == event ) {
				*i = 0;
				return;
			}
		}
// 		PANIC_PRINT("Trying to unregister a event, that is not registered.");
	}

	unsigned long TimeManager::getTime() const {
		return m_current_time;
	}

	unsigned long TimeManager::getTimeDelta() const {
		return m_time_delta;
	}

	double TimeManager::getAverageFrameTime() const {
		return m_average_frame_time;
	}

	void TimeManager::printStatistics() const {
		std::cout 
			<< "Timers:   " << m_events_list.size() << "\n";
	}

} //FIFE


