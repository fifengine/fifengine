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
/***************************************************************************
 * Note! FIFE event channel borrows heavily from Guichan library           *
 * Many of the classes and their implementation is almost identical to     *
 * ones that can be found from Guichan release 0.6                         *
***************************************************************************/


#ifndef FIFE_EVENTCHANNEL_IEVENT_H
#define FIFE_EVENTCHANNEL_IEVENT_H

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
	class IEventSource;

	/**  Base interface for all events
	 */
	class IEvent {
	public:

		/** Marks the event as consumed.
		 */
		virtual void consume() = 0;

		/** Checks if the event is consumed.
         * @return true if the event is consumed, false otherwise.
		 */
		virtual bool isConsumed() = 0;

		/** Gets the source of the event.
		 */
		virtual IEventSource* getSource() = 0;

		/** Gets the timestamp of the event
		 */
		virtual int getTimeStamp() = 0;

		/** Gets the debugstring of the event
		 */
		virtual std::string getDebugString() const = 0;

		/** Gets the name of the event
		 */
		virtual const std::string& getName() const = 0;

		virtual ~IEvent() {}

	};

} //FIFE

#endif
