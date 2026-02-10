/***************************************************************************
 *   Copyright (C) 2005-2019 by the FIFE team                              *
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
#include "event.h"

namespace FIFE {

	/**  Base class for input events (like mouse and keyboard)
	 */
	class InputEvent: public Event {
	public:
		/** Constructor.
		 */
		InputEvent():
			Event(),
			m_consumedByWidgets(false),
			m_isShiftPressed(false),
			m_isControlPressed(false),
			m_isAltPressed(false),
			m_isMetaPressed(false) {}

		/** Destructor.
		 */
		~InputEvent() {}

		/** Checks whether alt is pressed.
		 */
		virtual bool isAltPressed() const { return m_isAltPressed; }

		/** Sets alt to pressed.
		 */
		virtual void setAltPressed(bool pressed) { m_isAltPressed = pressed; }

		/** Checks whether control is pressed.
		 */
		virtual bool isControlPressed() const { return m_isControlPressed; }

		/** Sets control to pressed.
		 */
		virtual void setControlPressed(bool pressed) { m_isControlPressed = pressed; }

		/** Checks whether meta is pressed.
		 */
		virtual bool isMetaPressed() const { return m_isMetaPressed; }

		/** Sets meta to pressed.
		 */
		virtual void setMetaPressed(bool pressed) { m_isMetaPressed = pressed; }

		/** Checks whether shift is pressed.
		 */
		virtual bool isShiftPressed() const { return m_isShiftPressed; }

		/** Sets shift to pressed.
		 */
		virtual void setShiftPressed(bool pressed) { m_isShiftPressed = pressed; }

		/** Marks events as consumed by widget library.
		 */
		virtual void consumedByWidgets() { m_consumedByWidgets = true; }

		/** Checks whether event is consumed by widget library.
		 */
		virtual bool isConsumedByWidgets() const { return m_consumedByWidgets; }

		/** Marks events as consumed.
		 */
		virtual void consume() { Event::consume(); }

		/** Checks whether event is consumed.
		 */
		virtual bool isConsumed() const { return Event::isConsumed(); }

		/** Gets the source of the event.
		 */
		virtual IEventSource* getSource() const { return Event::getSource(); }

		/** Sets the source of the event.
		 */
		virtual void setSource(IEventSource* source) { Event::setSource(source); }

		/** Gets the timestamp of the event.
		 */
		virtual int32_t getTimeStamp() const { return Event::getTimeStamp(); }

		/** Sets the timestamp of the event.
		 */
		virtual void setTimeStamp(int32_t timestamp ) { Event::setTimeStamp(timestamp); }

		/** Gets the name of the event.
		 */
		virtual const std::string& getName() const {
			const static std::string eventName("InputEvent");
			return eventName;
		}

		/** Gets the debugstring of the event.
		 */
		virtual std::string getDebugString() const { return Event::getDebugString(); }

		/** Gets attribute string of the event.
		 */
		virtual std::string getAttrStr() const {
			std::stringstream ss;
			ss << Event::getAttrStr() << std::endl;
			ss << "shift = " << m_isShiftPressed << ", ";
			ss << "ctrl = " << m_isControlPressed << ", ";
			ss << "alt = " << m_isAltPressed << ", ";
			ss << "meta = " << m_isMetaPressed;
			return  ss.str();
		}


	private:
		bool m_consumedByWidgets;
		bool m_isShiftPressed;
		bool m_isControlPressed;
		bool m_isAltPressed;
		bool m_isMetaPressed;
	};
} //FIFE

#endif
