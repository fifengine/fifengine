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

#include "timemanager.h"
#include "timeevent.h"

#include <SDL.h>

namespace FIFE {

	TimeManager::TimeManager() {
		m_current_time	= SDL_GetTicks();
		m_time_delta	= 1;
	}

	TimeManager::~TimeManager() {
		unregisterAllEvents();
	}

	void TimeManager::update() {
		// Set new current time, time delta and framerate.
		m_time_delta = m_current_time;
		m_current_time = SDL_GetTicks();
		m_time_delta = m_current_time - m_time_delta;

		m_average_frame_time = (m_average_frame_time + static_cast<double>(m_time_delta)) / 2.0f;

		// Remove dead events and clear dead list.
		for (std::list<TimeEvent*>::const_iterator Itr = m_dead_events.begin(); Itr != m_dead_events.end(); ++Itr) {
			m_events_list.remove(*Itr);
		}
		m_dead_events.clear();

		// Update live events.
		for (std::list<TimeEvent*>::iterator Itr = m_events_list.begin(); Itr != m_events_list.end(); ++Itr) {
			(*Itr)->managerUpdateEvent(m_current_time);
		}		
	}

	void TimeManager::registerEvent(TimeEvent* event) {
		// Register.
		m_events_list.push_back(event);
	}

	void TimeManager::unregisterEvent(TimeEvent* event) {
		// Unregister.
		m_dead_events.push_back(event);
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

	void TimeManager::unregisterAllEvents() {
		m_events_list.clear();
		m_dead_events.clear();
	}

} //FIFE


