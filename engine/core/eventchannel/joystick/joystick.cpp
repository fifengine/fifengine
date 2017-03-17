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

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"

#include "joystick.h"

namespace FIFE {
	static Logger _log(LM_EVTCHANNEL);

	Joystick::Joystick(int32_t joystickId, int32_t deviceIndex):
		m_joystickHandle(NULL),
		m_controllerHandle(NULL),
		m_instanceId(-1),
		m_joystickId(joystickId),
		m_deviceIndex(deviceIndex),
		m_guidStr(""),
		m_name("") {

	}

	Joystick::~Joystick() {
		close();

	}

	int32_t Joystick::getInstanceId() const {
		return static_cast<int32_t>(m_instanceId);
	}

	int32_t Joystick::getJoystickId() const {
		return m_joystickId;
	}

	void Joystick::setDeviceIndex(int32_t deviceIndex) {
		m_deviceIndex = deviceIndex;
	}

	int32_t Joystick::getDeviceIndex() const {
		return m_deviceIndex;
	}

	std::string Joystick::getGuid() {
		return m_guidStr;
	}

	std::string Joystick::getName() {
		return m_name;
	}

	void Joystick::open() {
		if (m_joystickHandle) {
			close();
		}

		m_joystickHandle = SDL_JoystickOpen(m_deviceIndex);
		if (m_joystickHandle) {
			m_instanceId = SDL_JoystickInstanceID(m_joystickHandle);

			char tmp[33];
			SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(m_deviceIndex);
			SDL_JoystickGetGUIDString(guid, tmp, sizeof(tmp));
			m_guidStr = std::string(tmp);

			openController();
			const char* name = SDL_JoystickNameForIndex(m_deviceIndex);
			if (isController() && !name) {
				name = SDL_GameControllerNameForIndex(m_deviceIndex);
			}
			m_name = std::string(name);
		} else {
			throw SDLException(SDL_GetError());
		}
	}

	void Joystick::close() {
		closeController();
		if (m_joystickHandle) {
			SDL_JoystickClose(m_joystickHandle);
			m_joystickHandle = NULL;
		}
		m_instanceId = -1;
		m_deviceIndex = -1;
		m_guidStr = "";
		m_name = "";
	}

	bool Joystick::isConnected() const {
		return m_joystickHandle && SDL_JoystickGetAttached(m_joystickHandle);
	}

	bool Joystick::isController() const {
		return m_controllerHandle != NULL;
	}

	void Joystick::openController() {
		closeController();
		if (!SDL_IsGameController(m_deviceIndex)) {
			return;
		}

		m_controllerHandle = SDL_GameControllerOpen(m_deviceIndex);
	}

	void Joystick::closeController() {
		if (m_controllerHandle) {
			SDL_GameControllerClose(m_controllerHandle);
			m_controllerHandle = NULL;
		}
	}

}
