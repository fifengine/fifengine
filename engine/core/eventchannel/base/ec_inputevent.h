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

#ifndef FIFE_EVENTCHANNEL_INPUTEVENT_H
#define FIFE_EVENTCHANNEL_INPUTEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "ec_event.h"

namespace FIFE {

	/**  Base class for input events (like mouse and keyboard)
	 */
	class InputEvent: public Event {
	public:
		/** Constructor.
		*/
		InputEvent():
			Event(),
			m_consumedbywidgets(false),
			m_isshiftpressed(false),
			m_iscontrolpressed(false),
			m_isaltpressed(false),
			m_ismetapressed(false) {}

		/** Destructor.
		*/
		~InputEvent() {}

		/** Checks whether alt is pressed.
		 */
		virtual bool isAltPressed() const { return m_isaltpressed; }
		virtual void setAltPressed(bool pressed) { m_isaltpressed = pressed; }

		/** Checks whether control is pressed.
		 */
		virtual bool isControlPressed() const { return m_iscontrolpressed; }
		virtual void setControlPressed(bool pressed) { m_iscontrolpressed = pressed; }

		/** Checks whether meta is pressed.
		 */
		virtual bool isMetaPressed() const { return m_ismetapressed; }
		virtual void setMetaPressed(bool pressed) { m_ismetapressed = pressed; }

		/** Checks whether shift is pressed.
		 */
		virtual bool isShiftPressed() const { return m_isshiftpressed; }
		virtual void setShiftPressed(bool pressed) { m_isshiftpressed = pressed; }

		/** Marks events as consumed by widget library.
		 */
		virtual void consumedByWidgets() { m_consumedbywidgets = true; }
		virtual bool isConsumedByWidgets() const { return m_consumedbywidgets; }

		virtual void consume() { Event::consume(); }
		virtual bool isConsumed() const { return Event::isConsumed(); }
		virtual IEventSource* getSource() const { return Event::getSource(); }
		virtual void setSource(IEventSource* source) { Event::setSource(source); }
		virtual int32_t getTimeStamp() const { return Event::getTimeStamp(); }
		virtual void setTimeStamp(int32_t timestamp ) { Event::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("InputEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return Event::getDebugString(); }

		virtual std::string getAttrStr() const {
			std::stringstream ss;
			ss << Event::getAttrStr() << std::endl;
			ss << "shift = " << m_isshiftpressed << ", ";
			ss << "ctrl = " << m_iscontrolpressed << ", ";
			ss << "alt = " << m_isaltpressed << ", ";
			ss << "meta = " << m_ismetapressed;
			return  ss.str();
		}


	private:
		bool m_consumedbywidgets;
		bool m_isshiftpressed;
		bool m_iscontrolpressed;
		bool m_isaltpressed;
		bool m_ismetapressed;
	};
} //FIFE

#endif
