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

#ifndef FIFE_EVENTCHANNEL_MOUSEEVENT_H
#define FIFE_EVENTCHANNEL_MOUSEEVENT_H

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

namespace FIFE {

	/**  Class for mouse events
	 */
	class MouseEvent: public InputEvent {
	public:
		/**
		 * Mouse event types.
		 */
		enum MouseEventType
		{
			UNKNOWN_EVENT = -1,
			MOVED = 0,
			PRESSED,
			RELEASED,
			WHEEL_MOVED_DOWN,
			WHEEL_MOVED_UP,
			WHEEL_MOVED_RIGHT,
			WHEEL_MOVED_LEFT,
			CLICKED,
			ENTERED,
			EXITED,
			DRAGGED
		};

		/**
		 * Mouse button types.
		 */
		enum MouseButtonType
		{
			EMPTY = 0,
			LEFT = 1,
			RIGHT = 2,
			MIDDLE = 4,
			X1 = 8,
			X2 = 16,
			UNKNOWN_BUTTON = 32
		};


		/** Constructor.
		*/
		MouseEvent():
			InputEvent(),
			m_eventtype(UNKNOWN_EVENT),
			m_buttontype(UNKNOWN_BUTTON),
			m_x(-1),
			m_y(-1) {}

		/** Destructor.
		*/
		virtual ~MouseEvent() {}

		/**
		 * Gets the button of the mouse event.
		 * @return the button of the mouse event.
		 */
		MouseButtonType getButton() const { return m_buttontype; }
		void setButton(MouseButtonType type) { m_buttontype = type; }

		/**
		 * Gets the type of the event.
		 * @return the type of the event.
		 */
		MouseEventType getType() const { return m_eventtype; }
		void setType(MouseEventType type) { m_eventtype = type; }

		/**
		 * Gets the x coordinate of the mouse event. The coordinate is relative to
		 * the source event source.
		 * @return the x coordinate of the mouse event.
		 */
		int32_t getX() const { return m_x; }
		void setX(int32_t x) { m_x = x; }

		/**
		 * Gets the y coordinate of the mouse event. The coordinate is relative to
		 * the source event source.
		 * @return the y coordinate of the mouse event.
		 */
		int32_t getY() const { return m_y; }
		void setY(int32_t y) { m_y = y; }

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
			const static std::string eventName("MouseEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return InputEvent::getDebugString(); }
		virtual std::string getAttrStr() const {
			std::stringstream ss;
			ss << InputEvent::getAttrStr() << std::endl;
			ss << "event = " << mouseEventType2str(m_eventtype) << ", ";
			ss << "button = " << mouseButtonType2str(m_buttontype) << ", ";
			ss << "x = " << m_x << ", ";
			ss << "y = " << m_y;
			return  ss.str();
		}

		/** Returns string representation of given event type
		*/
		inline static std::string mouseEventType2str(MouseEventType t)  {
			std::string s("unknown");
			switch (t) {
				case MouseEvent::MOVED:
					s = "moved";
					break;
				case MouseEvent::PRESSED:
					s = "pressed";
					break;
				case MouseEvent::RELEASED:
					s = "released";
					break;
				case MouseEvent::WHEEL_MOVED_DOWN:
					s = "wheel_moved_down";
					break;
				case MouseEvent::WHEEL_MOVED_UP:
					s = "wheel_moved_up";
					break;
				case MouseEvent::WHEEL_MOVED_RIGHT:
					s = "wheel_moved_right";
					break;
				case MouseEvent::WHEEL_MOVED_LEFT:
					s = "wheel_moved_left";
					break;
				case MouseEvent::CLICKED:
					s = "clicked";
					break;
				case MouseEvent::ENTERED:
					s = "entered";
					break;
				case MouseEvent::EXITED:
					s = "excited";
					break;
				case MouseEvent::DRAGGED:
					s = "dragged";
					break;
				default:
					break;
			}
			return s;
		}

		/** Returns string representation of given button type
		*/
		inline static std::string mouseButtonType2str(MouseButtonType t) {
			std::string s("unknown");
			switch (t) {
				case MouseEvent::EMPTY:
					s = "empty";
					break;
				case MouseEvent::LEFT:
					s = "left";
					break;
				case MouseEvent::RIGHT:
					s = "right";
					break;
				case MouseEvent::MIDDLE:
					s = "middle";
					break;
				case MouseEvent::X1:
					s = "x1";
					break;
				case MouseEvent::X2:
					s = "x2";
					break;
				case MouseEvent::UNKNOWN_BUTTON:
					s = "unknown button";
					break;
				default:
					break;
			}
			return s;
		}



	private:
		MouseEventType m_eventtype;
		MouseButtonType m_buttontype;
		int32_t m_x;
 		int32_t m_y;

	};

} //FIFE

#endif
