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

#ifndef FIFE_EVENTCHANNEL_INPUTEVENT_H
#define FIFE_EVENTCHANNEL_INPUTEVENT_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "ec_event.h"
#include "ec_iinputevent.h"

namespace FIFE {

	/**  Base class for input events (like mouse and keyboard)
	 */
	class InputEvent: public Event, public IInputEvent {
	public:
        /** Constructor.
         */
		InputEvent(): 
			Event(), 
			m_isshiftpressed(false), 
			m_iscontrolpressed(false), 
			m_isaltpressed(false), 
			m_ismetapressed(false) {};

        /** Destructor.
         */
		~InputEvent() {}

		virtual bool isAltPressed() const { return m_isaltpressed; }
		virtual void setAltPressed(bool pressed) { m_isaltpressed = pressed; }
		virtual bool isControlPressed() const { return m_iscontrolpressed; }
		virtual void setControlPressed(bool pressed) { m_iscontrolpressed = pressed; }
		virtual bool isMetaPressed() const { return m_ismetapressed; }
		virtual void setMetaPressed(bool pressed) { m_ismetapressed = pressed; }
		virtual bool isShiftPressed() const { return m_isshiftpressed; }
		virtual void setShiftPressed(bool pressed) { m_isshiftpressed = pressed; }

		virtual void consume() { Event::consume(); }
		virtual bool isConsumed() { return Event::isConsumed(); }
		virtual IEventSource* getSource() { return Event::getSource(); }
		virtual void setSource(IEventSource* source) { Event::setSource(source); }
		virtual int getTimeStamp() { return Event::getTimeStamp(); }
		virtual void setTimeStamp(int timestamp ) { Event::setTimeStamp(timestamp); }


	private:
		bool m_isshiftpressed;
		bool m_iscontrolpressed;
		bool m_isaltpressed;
		bool m_ismetapressed;
	};
} //FIFE

#endif
