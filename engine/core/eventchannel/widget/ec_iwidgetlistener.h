/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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

#ifndef FIFE_EVENTCHANNEL_IWIDGETLISTENER_H
#define FIFE_EVENTCHANNEL_IWIDGETLISTENER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "ec_iwidgetevent.h"

namespace FIFE {

	/**  Listener of widget events.
	 * To be able to listen for widget events you must make a class which inherits 
	 * from this class and implements its functions.
	 */
	class IWidgetListener {
	public:
		/**
		 * Called when an action is recieved from a Widget. It is used
		 * to be able to recieve a notification that an action has
		 * occured.
		 * @param evy the event of the action.
		 */
		virtual void onWidgetAction(IWidgetEvent& evt) = 0;

		virtual ~IWidgetListener() {}
	};
} //FIFE

#endif
