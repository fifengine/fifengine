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

#ifndef FIFE_EVENTCHANNEL_JOYSTICKEVENT_H
#define FIFE_EVENTCHANNEL_JOYSTICKEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/base/inputevent.h"
#include "eventchannel/source/ieventsource.h"

#include "joystick.h"

namespace FIFE {

	/**  Class for Joystick events
	 */
	class JoystickEvent: public InputEvent {
	public:
		enum JoystickEventType {
			UNKNOWN_EVENT = 0,
			AXIS_MOTION,
			HAT_MOTION,
			BUTTON_PRESSED,
			BUTTON_RELEASED,
			DEVICE_ADDED,
			DEVICE_REMOVED
		};

		/** Constructor
		 */
		JoystickEvent():
			InputEvent(),
			m_eventType(UNKNOWN_EVENT),
			m_instanceId(-1),
			m_axis(-1),
			m_axisValue(0),
			m_hat(-1),
			m_hatValue(-1),
			m_button(-1),
			m_controller(false) {}

		/** Destructor.
		 */
		virtual ~JoystickEvent() {}

		JoystickEventType getType() const { return m_eventType; }
		void setType(JoystickEventType type) { m_eventType = type; }

		int32_t getInstanceId() const { return m_instanceId; }
		void setInstanceId(int32_t id) { m_instanceId = id; }

		int8_t getAxis() const { return m_axis; }
		void setAxis(int8_t axis) { m_axis = axis; }

		float getAxisValue() const { return m_axisValue; }
		void setAxisValue(float value) { m_axisValue = value; }

		int8_t getHat() const { return m_hat; }
		void setHat(int8_t hat) { m_hat = hat; }

		int8_t getHatValue() const { return m_hatValue; }
		void setHatValue(int8_t value) { m_hatValue = value; }

		int8_t getButton() const { return m_button; }
		void setButton(int8_t button) { m_button = button; }

		bool isController() const { return m_controller; }
		void setController(bool value) { m_controller = value; }

		virtual void consume() { InputEvent::consume(); }
		virtual bool isConsumed() const { return InputEvent::isConsumed(); }
		virtual void consumedByWidgets() { InputEvent::consumedByWidgets(); }
		virtual bool isConsumedByWidgets() const { return InputEvent::isConsumedByWidgets(); }
		virtual IEventSource* getSource() const { return InputEvent::getSource(); }
		virtual void setSource(IEventSource* source) { InputEvent::setSource(source); }
		virtual int32_t getTimeStamp() const { return InputEvent::getTimeStamp(); }
		virtual void setTimeStamp(int32_t timestamp ) { InputEvent::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("JostickEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return InputEvent::getDebugString(); }

	private:
		//! Event type
		JoystickEventType m_eventType;
		//! Joystick instance id
		int32_t m_instanceId;
		//! Axis index
		int8_t m_axis;
		//! Axis value
		float m_axisValue;
		//! Hat index
		int8_t m_hat;
		//! Hat value
		int8_t m_hatValue;
		//! Button index
		int8_t m_button;
		//! Is gamecontroller
		bool m_controller;
	};

} //FIFE

#endif
