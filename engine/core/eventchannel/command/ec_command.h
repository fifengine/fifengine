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

#ifndef FIFE_EVENTCHANNEL_COMMAND_H
#define FIFE_EVENTCHANNEL_COMMAND_H

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
#include "ec_commandids.h"

namespace FIFE {

	/**  Class for commands
	 * Commands are arbitrary events e.g. send from one subsystem to another
	 */
	class Command: public Event {
	public:
		/** Constructor.
		 */
		Command():
			Event(),
			m_commandtype(CMD_UNKNOWN),
			m_code(0) {}

		/** Destructor.
		 */
		virtual ~Command() {}

		/** Gets the type of this command
		 * @return type of this command
		 */
		CommandType getCommandType() const { return m_commandtype; }
		void setCommandType(CommandType type) { m_commandtype = type; }

		/** Gets the code of this command. Meaning of code depends on the command type
		 * @return code of this command
		 */
		int32_t getCode() const { return m_code; }
		void setCode(int32_t code) { m_code = code; }

		virtual void consume() { Event::consume(); }
		virtual bool isConsumed() const { return Event::isConsumed(); }
		virtual IEventSource* getSource() const { return Event::getSource(); }
		virtual void setSource(IEventSource* source) { Event::setSource(source); }
		virtual int32_t getTimeStamp() const { return Event::getTimeStamp(); }
		virtual void setTimeStamp(int32_t timestamp ) { Event::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("Command");
			return eventName;
		}
		virtual std::string getDebugString() const { return Event::getDebugString(); }


	private:
		CommandType m_commandtype;
		int32_t m_code;
	};
} //FIFE

#endif
