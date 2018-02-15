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

#ifndef FIFE_EVENTCHANNEL_IDROP_CONTROLLER_H
#define FIFE_EVENTCHANNEL_IDROP_CONTROLLER_H

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
	class IDropListener;

	/**  Controller provides a way to receive events from the system
	 * Using this interface, clients can subscribe themselves to receive events
	 */
	class IDropController {
	public:

		/** Adds a listener to the back of the listener deque
		 * Listener will be notified via the corresponding events
		 * @param listener listener to add
		 */
		virtual void addDropListener(IDropListener* listener) = 0;

		/** Adds a listener to the front of the listener deque
		 * Listener will be notified via the corresponding events
		 * @param listener listener to add
		 */
		virtual void addDropListenerFront(IDropListener* listener) = 0;

		/** Removes an added listener from the controller.
		 * Listener will not be notified anymore via the corresponding events
		 * @param listener listener to remove
		 */
		virtual void removeDropListener(IDropListener* listener) = 0;

		virtual ~IDropController() {}
	};

} //FIFE

#endif
