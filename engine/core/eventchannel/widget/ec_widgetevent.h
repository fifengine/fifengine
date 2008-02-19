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

#ifndef FIFE_EVENTCHANNEL_WIDGETEVENT_H
#define FIFE_EVENTCHANNEL_WIDGETEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/base/ec_event.h"
#include "ec_iwidgetevent.h"

namespace FIFE {

	/**  Interface for widget events
	 * Events are sent by different UI widgets
	 */
	class WidgetEvent: public Event, public IWidgetEvent {
	public:
        /** Constructor.
         */
		WidgetEvent(): 
			Event(),
			m_id("") {}

        /** Destructor.
         */
		~WidgetEvent() {}

		const std::string& getId() const { return m_id; }
		void setId(const std::string& id) { m_id = id; }

		virtual void consume() { Event::consume(); }
		virtual bool isConsumed() const { return Event::isConsumed(); }
		virtual IEventSource* getSource() { return Event::getSource(); }
		virtual void setSource(IEventSource* source) { Event::setSource(source); }
		virtual gcn::Widget* getSourceWidget() { return Event::getSourceWidget(); }
		virtual void setSourceWidget(gcn::Widget* widget) { Event::setSourceWidget(widget); }
		virtual int getTimeStamp() const { return Event::getTimeStamp(); }
		virtual void setTimeStamp(int timestamp ) { Event::setTimeStamp(timestamp); }
		virtual const std::string& getName() const {
			const static std::string eventName("WidgetEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return Event::getDebugString(); }

	private:
		std::string m_id;
	};

} //FIFE

#endif
