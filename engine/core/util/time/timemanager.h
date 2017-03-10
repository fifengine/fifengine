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

#ifndef FIFE_TIMEMANAGER_H
#define FIFE_TIMEMANAGER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/singleton.h"
#include "util/base/fife_stdint.h"

namespace FIFE {

	class TimeEvent;

	/** Time Manager
	 *
	 * This class is in charge of storing the current time,
	 * average frame time, as well as controlling periodic events.
	 * Users of this class will have to manually register and
	 * unregister events.
	 *
	 * @see TimeEvent
	 */
	class TimeManager : public DynamicSingleton<TimeManager> {
	public:
		/** Default constructor.
		 */
		TimeManager();

		/** Destructor.
		 */
		virtual ~TimeManager();

		/** Called once a frame and updates the timer objects and events.
		 */
		void update();

		/** Adds a TimeEvent.
		 *
		 * The event will be updated regularly, depending on its settings.
		 * @param event The TimeEvent object to be added.
		 */
		void registerEvent(TimeEvent* event);

		/** Removes a TimeEvent.
		 *
		 * Removes an event from the list. It will not be deleted.
		 * @param event The TimeEvent object to be removed.
		 */
		void unregisterEvent(TimeEvent* event);

		/** Get the time.
		 *
		 * @return The time in milliseconds.
		 */
		uint32_t getTime() const;

		/** Get the time since the last frame.
		 *
		 * @return Time since last frame in milliseconds.
		 */
		uint32_t getTimeDelta() const;

		/** Gets average frame time
		 *
		 * @return Average frame time in milliseconds.
		 */
		double getAverageFrameTime() const;

		/** Prints Timer statistics
		 */
		void printStatistics() const;

	private:
		/// Current time in milliseconds.
		uint32_t m_current_time;
		/// Time since last frame in milliseconds.
		uint32_t m_time_delta;
		/// Average frame time in milliseconds.
		double m_average_frame_time;

		/// List of active TimeEvents.
		std::vector<TimeEvent*> m_events_list;
	};

}//FIFE

#endif
