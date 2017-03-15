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

#ifndef FIFE_EVENTCHANNEL_JOYSTICKMANAGER_H
#define FIFE_EVENTCHANNEL_JOYSTICKMANAGER_H

// Standard C++ library includes
//
#include <vector>
#include <deque>
#include <map>
#include <list>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "loaders/native/input/controllermappingloader.h"
#include "savers/native/input/controllermappingsaver.h"
#include "util/base/fife_stdint.h"

#include "ijoystickcontroller.h"
#include "ijoysticklistener.h"
#include "joystickevent.h"
#include "joystick.h"


namespace FIFE {

	//class JoystickEvent;
	//class ControllerMappingLoader;
	//class ControllerMappingSaver;


	/**  Joystick Manager manages all events related to Joysticks and Gamecontrollers
	 */
	class JoystickManager:
		public IJoystickController,
		public IEventSource {
	public:
		/** Constructor.
		 */
		JoystickManager();

		/** Destructor
		 */
		virtual ~JoystickManager();

		void addJoystick(int32_t deviceIndex);
		void removeJoystick(Joystick* joystick);
		Joystick* getJoystick(int32_t instanceId);
		uint8_t getJoystickCount() const;

		void loadMapping(const std::string& path);
		void saveMapping(const std::string& path);

		// Implementation from IJoystickController
		void addJoystickListener(IJoystickListener* listener);
		void addJoystickListenerFront(IJoystickListener* listener);
		void removeJoystickListener(IJoystickListener* listener);

		void processJoystickEvent(SDL_Event event);
		void fillJoystickEvent(const SDL_Event& sdlevt, JoystickEvent& joyevt);
		void dispatchJoystickEvent(JoystickEvent& evt);

		EventSourceType getEventSourceType();
	private:
		ControllerMappingLoader m_mappingLoader;
		ControllerMappingSaver m_mappingSaver;

		//! All active / connected Joysticks.
		std::vector<Joystick*> m_activeJoysticks;

		//! All "known" Joysticks. Useful if a user reconnect a Joystick.
		std::list<Joystick*> m_joysticks;

		//! Each sort of gamepad have a GUID from SDL. Indicates if gamepad with given GUID is connected.
		std::map<std::string, bool> m_gamepadGuids;

		std::deque<IJoystickListener*> m_joystickListeners;
		std::deque<IJoystickListener*> m_pendingJoystickListeners;
		std::deque<IJoystickListener*> m_pendingJoystickListenersFront;
		std::deque<IJoystickListener*> m_pendingJoystickDeletions;
	};
} //FIFE

#endif
