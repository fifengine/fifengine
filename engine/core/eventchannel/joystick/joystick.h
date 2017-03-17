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

#ifndef FIFE_EVENTCHANNEL_JOYSTICK_H
#define FIFE_EVENTCHANNEL_JOYSTICK_H

// Standard C++ library includes

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {

	/** Represents a Joystick.
	 */
	class Joystick {
	public:

		// Hat positions.
		enum Hat {
			HAT_INVALID = -1,
			HAT_CENTERED = 0,
			HAT_UP = 1,
			HAT_RIGHT = 2,
			HAT_DOWN = 4,
			HAT_LEFT = 8,
			HAT_RIGHTUP = 3,
			HAT_RIGHTDOWN = 6,
			HAT_LEFTUP = 9,
			HAT_LEFTDOWN = 12
		};

		// The list of axes available from a controller.
		enum ContollerAxis {
			CONTOLLER_AXIS_INVALID = -1,
			CONTOLLER_AXIS_LEFTX,
			CONTOLLER_AXIS_LEFTY,
			CONTOLLER_AXIS_RIGHTX,
			CONTOLLER_AXIS_RIGHTY,
			CONTOLLER_AXIS_TRIGGERLEFT,
			CONTOLLER_AXIS_TRIGGERRIGHT,
			CONTOLLER_AXIS_MAX
		};

		// The list of buttons available from a controller.
		enum ControllerButton {
			CONTOLLER_BUTTON_INVALID = -1,
			CONTOLLER_BUTTON_A,
			CONTOLLER_BUTTON_B,
			CONTOLLER_BUTTON_X,
			CONTOLLER_BUTTON_Y,
			CONTOLLER_BUTTON_BACK,
			CONTOLLER_BUTTON_GUIDE,
			CONTOLLER_BUTTON_START,
			CONTOLLER_BUTTON_LEFTSTICK,
			CONTOLLER_BUTTON_RIGHTSTICK,
			CONTOLLER_BUTTON_LEFTSHOULDER,
			CONTOLLER_BUTTON_RIGHTSHOULDER,
			CONTOLLER_BUTTON_DPAD_UP,
			CONTOLLER_BUTTON_DPAD_DOWN,
			CONTOLLER_BUTTON_DPAD_LEFT,
			CONTOLLER_BUTTON_DPAD_RIGHT,
			CONTOLLER_BUTTON_MAX
		};

		/** Constructor
		 * @param joystickId The identifier for the Joystick.
		 * @param deviceIndex The index number from SDL2.
		 */
		Joystick(int32_t joystickId, int32_t deviceIndex);

		/** Destructor
		 */
		~Joystick();

		int32_t getInstanceId() const;

		int32_t getJoystickId() const;

		void setDeviceIndex(int32_t deviceIndex);

		int32_t getDeviceIndex() const;

		std::string getGuid();
		std::string getName();

		void open();
		void close();

		bool isConnected() const;
		bool isController() const;
		void openController();
		void closeController();


	private:
		SDL_Joystick* m_joystickHandle;
		SDL_GameController* m_controllerHandle;
		SDL_JoystickID m_instanceId;
		//SDL_JoystickGUID m_guid;
		int32_t m_joystickId;
		int32_t m_deviceIndex;
		std::string m_guidStr;
		std::string m_name;
	};

} //FIFE

#endif
