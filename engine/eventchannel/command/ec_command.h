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
#include "../base/ec_event.h"
#include "ec_icommand.h"
#include "ec_commandids.h"

namespace FIFE {

	/**  Class for commands
	 * Commands are arbitrary events e.g. send from one subsystem to another
	 */
	class Command: public Event, public ICommand {
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

		CommandType getCommandType() { return m_commandtype; }
		void setCommandType(CommandType type) { m_commandtype = type; }

		int getCode() { return m_code; }
		void setCode(int code) { m_code = code; }

		virtual void consume() { Event::consume(); }
		virtual bool isConsumed() const { return Event::isConsumed(); }
		virtual IEventSource* getSource() { return Event::getSource(); }
		virtual void setSource(IEventSource* source) { Event::setSource(source); }
		virtual int getTimeStamp() const { return Event::getTimeStamp(); }
		virtual void setTimeStamp(int timestamp ) { Event::setTimeStamp(timestamp); }

		virtual const std::string& getName() const {
			const static std::string eventName("Command");
			return eventName;
		}
		virtual std::string getDebugString() const { return Event::getDebugString(); }


	private:
		CommandType m_commandtype;
		int m_code;
		void* m_data1;
		void* m_data2;
	};
} //FIFE

#endif
