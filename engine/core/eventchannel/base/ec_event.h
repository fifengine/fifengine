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
/***************************************************************************
 * Note! FIFE event channel borrows heavily from ideas pf Guichan library  *
 * version 0.6                                                             *
 ***************************************************************************/


#ifndef FIFE_EVENTCHANNEL_EVENT_H
#define FIFE_EVENTCHANNEL_EVENT_H

// Standard C++ library includes
//
#include <string>
#include <sstream>

// 3rd party library includes
//
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/source/ec_ieventsource.h"

namespace FIFE {
	/**  Base class for all events
	 */
	class Event {
	public:
		/** Constructor.
		*/
		Event():
			m_isconsumed(false),
			m_eventsource(NULL),
			m_timestamp(SDL_GetTicks()) {}

		/** Destructor.
		*/
		virtual ~Event() {}

		/** Marks the event as consumed.
		 */
		virtual void consume() { m_isconsumed = true; }

		/** Checks if the event is consumed.
		 * @return true if the event is consumed, false otherwise.
		 */
		virtual bool isConsumed() const { return m_isconsumed; }

		/** Gets the source of the event.
		 */
		virtual IEventSource* getSource() const { return m_eventsource; }

		/** Sets the source of the event.
		 */
		virtual void setSource(IEventSource* source) { m_eventsource = source; }

		/** Gets the timestamp of the event
		 */
		virtual int32_t getTimeStamp() const { return m_timestamp; }

		/** Sets the timestamp of the event
		 */
		virtual void setTimeStamp(int32_t timestamp ) { m_timestamp = timestamp; }

		/** Gets the name of the event
		 */
		virtual const std::string& getName() const {
			const static std::string eventName("Event");
			return eventName;
		}

		/** Gets attribute string of the event
		 */
		virtual std::string getAttrStr() const {
			std::stringstream ss;
			ss << "consumed = " << m_isconsumed << ", ";
			ss << "src = " << m_eventsource << ", ";
			ss << "timestamp = " << m_timestamp;
			return ss.str();
		}

		/** Gets the debugstring of the event
		 */
		virtual std::string getDebugString() const {
			std::stringstream ss;
			ss << getName() << std::endl;
			ss << getAttrStr() << std::endl;
			return ss.str();
		}

	private:
		bool m_isconsumed;
		IEventSource* m_eventsource;
		int32_t m_timestamp;
	};

} //FIFE

#endif
