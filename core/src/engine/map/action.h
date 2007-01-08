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

#ifndef FIFE_MAP_ACTION_H
#define FIFE_MAP_ACTION_H

// Standard C++ library includes

// Platform specific includes
#include "util/fifeint.h"

// 3rd party library includes
#include <boost/function.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace map {

	/** Helper Class for Complex Animation
	 */
	class Action {
		public:
			typedef boost::function1<void, const Action&> type_callback;

			/** Default Constructor
			 */
			Action()
				: actionId(0),
				direction(0),
				startCallback(),
				endCallback()
				{
			}

			Action(uint8_t _actionId, uint8_t _direction,
				const type_callback& startCB = type_callback(),
				const type_callback& endCB = type_callback())
				: actionId(_actionId),
				direction(_direction),
				startCallback(startCB),
				endCallback(endCB)
				{
			}
				
			uint8_t actionId;
			uint8_t direction;

			type_callback  startCallback;
			type_callback  endCallback;
	};
} }

#endif // ACTION_H
