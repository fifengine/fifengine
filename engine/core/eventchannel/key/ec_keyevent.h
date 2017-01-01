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

#ifndef FIFE_EVENTCHANNEL_KEYEVENT_H
#define FIFE_EVENTCHANNEL_KEYEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/base/ec_inputevent.h"
#include "eventchannel/source/ec_ieventsource.h"

#include "ec_key.h"

namespace FIFE {

	/**  Class for key events
	 */
	class KeyEvent: public InputEvent {
	public:
		enum KeyEventType {
			UNKNOWN = -1,
			PRESSED = 0,
			RELEASED
		};

		/** Constructor
		 */
		KeyEvent():
			InputEvent(),
			m_eventtype(UNKNOWN),
			m_isnumericpad(false),
			m_key(Key()) {}

		/** Destructor.
		 */
		virtual ~KeyEvent() {}

		KeyEventType getType() const { return m_eventtype; }
		void setType(KeyEventType type) { m_eventtype = type; }

		bool isNumericPad() const { return m_isnumericpad; }
		void setNumericPad(bool ispad) { m_isnumericpad = ispad; }

		const Key& getKey() const { return m_key; }
		void setKey(const Key& key) { m_key = key; }

		virtual bool isAltPressed() const { return InputEvent::isAltPressed(); }
		virtual void setAltPressed(bool pressed) { InputEvent::setAltPressed(pressed); }
		virtual bool isControlPressed() const { return InputEvent::isControlPressed(); }
		virtual void setControlPressed(bool pressed) { InputEvent::setControlPressed(pressed); }
		virtual bool isMetaPressed() const { return InputEvent::isMetaPressed(); }
		virtual void setMetaPressed(bool pressed) { InputEvent::setMetaPressed(pressed); }
		virtual bool isShiftPressed() const { return InputEvent::isShiftPressed(); }
		virtual void setShiftPressed(bool pressed) { InputEvent::setShiftPressed(pressed); }

		virtual void consume() { InputEvent::consume(); }
		virtual bool isConsumed() const { return InputEvent::isConsumed(); }
		virtual void consumedByWidgets() { InputEvent::consumedByWidgets(); }
		virtual bool isConsumedByWidgets() const { return InputEvent::isConsumedByWidgets(); }
		virtual IEventSource* getSource() const { return InputEvent::getSource(); }
		virtual void setSource(IEventSource* source) { InputEvent::setSource(source); }
		virtual int32_t getTimeStamp() const { return InputEvent::getTimeStamp(); }
		virtual void setTimeStamp(int32_t timestamp ) { InputEvent::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("KeyEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return InputEvent::getDebugString(); }

	private:
		KeyEventType m_eventtype;
		bool m_isnumericpad;
		Key m_key;
	};

} //FIFE

#endif
