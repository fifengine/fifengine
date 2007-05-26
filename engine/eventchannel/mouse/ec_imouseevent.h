/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

#ifndef FIFE_EVENTCHANNEL_IMOUSEEVENT_H
#define FIFE_EVENTCHANNEL_IMOUSEEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "../base/ec_iinputevent.h"

namespace FIFE {

	/**  Interface for mouse events
	 */
	class IMouseEvent: public IInputEvent {
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
			UNKNOWN_BUTTON = 8
		};

		/**
		 * Gets the x coordinate of the mouse event. The coordinate is relative to
		 * the source event source. 
		 * @return the x coordinate of the mouse event.
		 */
		virtual int getX() const = 0;

		/**
		 * Gets the y coordinate of the mouse event. The coordinate is relative to
		 * the source event source. 
		 * @return the y coordinate of the mouse event.
		 */
        virtual int getY() const = 0;

		/**
		 * Gets the type of the event.
		 * @return the type of the event.
		 */
		virtual MouseEventType getType() const = 0;

		/**
		 * Gets the button of the mouse event.
		 * @return the button of the mouse event.
		 */
		virtual MouseButtonType getButton() const = 0;

		virtual ~IMouseEvent() {}
	};

	inline const std::string mouseEventType2str(IMouseEvent::MouseEventType t) {
		std::string s("unknown");
		switch (t) {
			case IMouseEvent::MOVED:
				s = "moved";
				break;
			case IMouseEvent::PRESSED:
				s = "pressed";
				break;
			case IMouseEvent::RELEASED:
				s = "released";
				break;
			case IMouseEvent::WHEEL_MOVED_DOWN:
				s = "wheel moved down";
				break;
			case IMouseEvent::WHEEL_MOVED_UP:
				s = "wheel moved up";
				break;
			case IMouseEvent::CLICKED:
				s = "clicked";
				break;
			case IMouseEvent::ENTERED:
				s = "entered";
				break;
			case IMouseEvent::EXITED:
				s = "excited";
				break;
			case IMouseEvent::DRAGGED:
				s = "dragged";
				break;
			default:
				break;
		}
		return s;
	}

	inline const std::string mouseButtonType2str(IMouseEvent::MouseButtonType t) {
		std::string s("unknown");
		switch (t) {
			case IMouseEvent::EMPTY:
				s = "empty";
				break;
			case IMouseEvent::LEFT:
				s = "left";
				break;
			case IMouseEvent::RIGHT:
				s = "right";
				break;
			case IMouseEvent::MIDDLE:
				s = "middle";
				break;
			default:
				break;
		}
		return s;
	}

} //FIFE


#endif
