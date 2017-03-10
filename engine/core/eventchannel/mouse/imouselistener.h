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

#ifndef FIFE_EVENTCHANNEL_IMOUSELISTENER_H
#define FIFE_EVENTCHANNEL_IMOUSELISTENER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE {

	class MouseEvent;

	/**  Listener of mouse events.
	 * To be able to listen for mouse events you must make a class which inherits
	 * from this class and implements its functions.
	 */
	class IMouseListener {
	public:
		/**
		 * Called when the mouse has entered into the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseEntered(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse has exited the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseExited(MouseEvent& evt) = 0;

		/**
		 * Called when a mouse button has been pressed on the event source area.
		 * NOTE: A mouse press is NOT equal to a mouse click.
		 * @param evt describes the event.
		 */
		virtual void mousePressed(MouseEvent& evt) = 0;

		/**
		 * Called when a mouse button has been released on the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseReleased(MouseEvent& evt) = 0;

		/**
		 * Called when a mouse button is pressed and released (clicked) on
		 * the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseClicked(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse wheel has moved up on the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseWheelMovedUp(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse wheel has moved down on the event source area.
		 * @param evt MouseEvent that describes the event.
		 */
		virtual void mouseWheelMovedDown(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse wheel has moved right on the event source area.
		 * @param evt describes the event.
		 */
		virtual void mouseWheelMovedRight(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse wheel has moved left on the event source area.
		 * @param evt MouseEvent that describes the event.
		 */
		virtual void mouseWheelMovedLeft(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse has moved in the event source area and no mouse button
		 * has been pressed
		 * @param evt describes the event.
		 */
		virtual void mouseMoved(MouseEvent& evt) = 0;

		/**
		 * Called when the mouse has moved and the mouse has previously been
		 * pressed on the event source.
		 * @param evt describes the event.
		 */
		virtual void mouseDragged(MouseEvent& evt) = 0;

		virtual ~IMouseListener() {}
	};

} //FIFE

#endif
