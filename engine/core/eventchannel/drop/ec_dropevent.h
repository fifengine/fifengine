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

#ifndef FIFE_EVENTCHANNEL_DROPEVENT_H
#define FIFE_EVENTCHANNEL_DROPEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/base/ec_inputevent.h"
#include "eventchannel/source/ec_ieventsource.h"

namespace FIFE {

	/**  Class for drop events
	 */
	class DropEvent: public InputEvent {
	public:

		/** Constructor
		 */
		DropEvent():
			InputEvent(),
			m_path("") {}

		/** Destructor.
		 */
		virtual ~DropEvent() {}

		const std::string& getPath() const { return m_path; }
		void setPath(const std::string& path) { m_path = path; }

		virtual void consume() { InputEvent::consume(); }
		virtual bool isConsumed() const { return InputEvent::isConsumed(); }
		virtual void consumedByWidgets() { InputEvent::consumedByWidgets(); }
		virtual bool isConsumedByWidgets() const { return InputEvent::isConsumedByWidgets(); }
		virtual IEventSource* getSource() const { return InputEvent::getSource(); }
		virtual void setSource(IEventSource* source) { InputEvent::setSource(source); }
		virtual int32_t getTimeStamp() const { return InputEvent::getTimeStamp(); }
		virtual void setTimeStamp(int32_t timestamp ) { InputEvent::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("DropEvent");
			return eventName;
		}
		virtual std::string getDebugString() const { return InputEvent::getDebugString(); }

	private:
		std::string m_path;
	};

} //FIFE

#endif
