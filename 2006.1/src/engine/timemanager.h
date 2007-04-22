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

#ifndef FIFE_TIMEMANAGER_H
#define FIFE_TIMEMANAGER_H

#include <list>

#include "singleton.h"

namespace FIFE {

	class TimeEvent;

    /**  Time Manager.  
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
		 * 
		 * @param event The TimeEvent object to be added.
		 */
		void registerEvent(TimeEvent* event);

		/** Removes a TimeEvent.
		 *
		 * Removes an event from the list. It will not be deleted.
		 * 
		 * @param event The TimeEvent object to be removed.
		 */
		void unregisterEvent(TimeEvent* event);

		/** Get the time.
		 * 
		 * @return The time in milliseconds.
		 */
		unsigned long getTime() const;

		/** Time since the last frame.
		 * 
		 * @return Time since last frame in milliseconds.
		 */
		unsigned long getTimeDelta() const;

		/** Gets average frame time.
		 * 
		 * @return Average frame time in milliseconds.
		 */
		double getAverageFrameTime() const;

	private:
		/** Removes all TimeEvents.
		 *
		 * Removes all events from the list. They will not be deleted.
		 * Called by destructor.
		 */
		void unregisterAllEvents();

    private:
		// current time in milliseconds.
		unsigned long m_current_time;
		// time since last frame in milliseconds.
		unsigned long m_time_delta;
		// average frame time in milliseconds.
		double m_average_frame_time;

		// list of active TimeEvents.
		std::list<TimeEvent*> m_events_list;
		// list of TimeEvents to be removed from the events list.
		std::list<TimeEvent*> m_dead_events;
    };

}//FIFE

#endif
