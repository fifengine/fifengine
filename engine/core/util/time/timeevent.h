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

#ifndef FIFE_TIMEVENT_H
#define FIFE_TIMEVENT_H

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"

namespace FIFE {

	/** Interface for events to be registered with TimeManager.
	*
	* To register a class with TimeManager firstly derive a class
	* from this and override the updateEvent() function. updateEvent()
	* will be called periodically depending on the value of getPeriod()
	* which can be set using the constructor or setPeriod(). A value
	* of -1 will never be updated, 0 will updated every frame and a value
	* over 0 defines the number of milliseconds between updates.
	*
	* @see TimeManager
	*/
	class TimeEvent {
	public:
		/** Default constructor.
		 *
		 * @param period The period of the event. See class description.
		 */
		TimeEvent(int32_t period = -1);

		/** Destructor.
		 *
		 */
		virtual ~TimeEvent();

		/** Update function to be overridden by client.
		 *
		 * @param time Time delta.
		 */
		virtual void updateEvent(uint32_t time) = 0;

		/** Called by TimeManager to update the event.
		 *
		 * @param time Current time. Used To check if its time to update.
		 */
		void managerUpdateEvent(uint32_t time);

		/** Set the period of the event.
		 *
		 * @param period The period of the event. See class description.
		 */
		void setPeriod(int32_t period);

		/** Get the period of the event.
		 *
		 * @return The period of the event. See class description.
		 */
		int32_t getPeriod();

		/** Get the last time the event was updated.
		 *
		 * @return Time of last update.
		 */
		uint32_t getLastUpdateTime();

		/** Set the last time the event was updated.
		 *
		 * @param ms Time of last update.
		 */
		void setLastUpdateTime(uint32_t ms);

    private:
		// The period of the event. See the class description.
		int32_t m_period;

		// The last time the class was updated.
		uint32_t m_last_updated;
    };

}//FIFE

#endif
