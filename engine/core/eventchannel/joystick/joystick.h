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

		/** Constructor
		 * @param joystickId The identifier for the Joystick.
		 * @param deviceIndex The index number from SDL2.
		 */
		Joystick(int32_t joystickId, int32_t deviceIndex);

		/** Destructor
		 */
		~Joystick();

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
