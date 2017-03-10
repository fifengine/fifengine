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

#ifndef FIFE_EVENTCHANNEL_ITEXTLISTENER_H
#define FIFE_EVENTCHANNEL_ITEXTLISTENER_H

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
	class TextEvent;

	/**  Listener of text events.
	 * To be able to listen for text events you must make a class which inherits
	 * from this class and implements its functions.
	 */
	class ITextListener {
	public:
		/** Called on a text input event.
		 * @param evt TextEvent that describes the event.
		 */
		virtual void textInput(TextEvent& evt) = 0;

		/** Called on a text edit event.
		 * @param evt TextEvent that describes the event.
		 */
		virtual void textEdit(TextEvent& evt) = 0;

		virtual ~ITextListener() {}
	};

} //FIFE

#endif
