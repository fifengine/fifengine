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

#ifndef FIFE_EVENTCHANNEL_IKEYFILTER_H
#define FIFE_EVENTCHANNEL_IKEYFILTER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "ec_keyevent.h"

namespace FIFE {
	/**  Controller provides a way to receive events from the system
	 * Using this interface, clients can subscribe themselves to receive events
	 */
	class IKeyFilter {
	public:

		/** Check whether a keyevent should be filtered out.
		 * @param event They key event.
		 */
		virtual bool isFiltered(const KeyEvent& event) = 0;

		virtual ~IKeyFilter() {}
	};

} //FIFE

#endif
