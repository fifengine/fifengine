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

#ifndef FIFE_EVENTCHANNEL_IKEYLISTENER_H
#define FIFE_EVENTCHANNEL_IKEYLISTENER_H

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
	class KeyEvent;

	/**  Listener of key events.
	 * To be able to listen for key events you must make a class which inherits
	 * from this class and implements its functions.
	 */
	class IKeyListener {
	public:
		/** Called if a key is pressed
		 * If a key is held down the multiple key presses are generated.
		 * @param evt KeyEvent that describes the event.
		 */
		virtual void keyPressed(KeyEvent& evt) = 0;

		/** Called if a key is released
		 * @param evt KeyEvent that describes the event.
		 */
		virtual void keyReleased(KeyEvent& evt) = 0;

		virtual ~IKeyListener() {}
	};

} //FIFE

#endif
