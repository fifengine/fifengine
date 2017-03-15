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
//#include "loaders/native/input/controllermappingloader.h"
//#include "savers/native/input/controllermappingsaver.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"

#include "joystickmanager.h"

namespace FIFE {
	static Logger _log(LM_EVTCHANNEL);

	JoystickManager::JoystickManager() {
		// init joystick and controller systems
		if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0) {
			throw SDLException(SDL_GetError());
		}
		// create loader/saver for controller mappings
		m_mappingLoader = ControllerMappingLoader();
		m_mappingSaver = ControllerMappingSaver();

		for (int32_t i = 0; i < SDL_NumJoysticks(); ++i) {
			addJoystick(i);
		}

		// enable joystick and gamecontroller events
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_GameControllerEventState(SDL_ENABLE);
	}

	JoystickManager::~JoystickManager() {
		for (std::list<Joystick*>::iterator it = m_joysticks.begin(); it != m_joysticks.end(); ++it) {
			//(*it)->close();
			delete *it;
		}

		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	}

	void JoystickManager::addJoystick(int32_t deviceIndex) {
		Joystick* joystick = new Joystick(deviceIndex, m_joysticks.size());
		joystick->open();
		//m_joysticks.push_back(joystick);
		m_activeJoysticks.push_back(joystick);
	}

	void JoystickManager::removeJoystick(Joystick* joystick) {
		std::vector<Joystick*>::iterator it = std::find(m_activeJoysticks.begin(), m_activeJoysticks.end(), joystick);
		if (it != m_activeJoysticks.end()) {
			(*it)->close();
			m_activeJoysticks.erase(it);
		}
	}

	Joystick* JoystickManager::getJoystick(int32_t instanceId) {
		return NULL;
	}

	uint8_t JoystickManager::getJoystickCount() const {
		return static_cast<uint8_t>(m_activeJoysticks.size());
	}

	void JoystickManager::loadMapping(const std::string& path) {
	
	}

	void JoystickManager::saveMapping(const std::string& path) {
	
	}


	void JoystickManager::addJoystickListener(IJoystickListener* listener) {
		m_pendingJoystickListeners.push_back(listener);
	}

	void JoystickManager::addJoystickListenerFront(IJoystickListener* listener) {
		m_pendingJoystickListenersFront.push_back(listener);
	}

	void JoystickManager::removeJoystickListener(IJoystickListener* listener) {
		m_pendingJoystickListenersFront.push_back(listener);
	}

	void JoystickManager::processJoystickEvent(SDL_Event event) {
		JoystickEvent joyevent;
		joyevent.setSource(this);
		fillJoystickEvent(event, joyevent);
		dispatchJoystickEvent(joyevent);
	}

	void JoystickManager::fillJoystickEvent(const SDL_Event& sdlevt, JoystickEvent& joyevt) {
		if (sdlevt.type == SDL_JOYAXISMOTION || sdlevt.type == SDL_CONTROLLERAXISMOTION) {
			joyevt.setType(JoystickEvent::AXIS_MOTION);
			if (sdlevt.type == SDL_JOYAXISMOTION) {
				joyevt.setAxis(sdlevt.jaxis.axis);
				joyevt.setAxisValue(sdlevt.jaxis.value / 32768.0f);
			} else {
				joyevt.setAxis(sdlevt.caxis.axis);
				joyevt.setAxisValue(sdlevt.caxis.value / 32768.0f);
			}
		}
	}

	void JoystickManager::dispatchJoystickEvent(JoystickEvent& evt) {
		if (!m_pendingJoystickListeners.empty()) {
			std::deque<IJoystickListener*>::iterator i = m_pendingJoystickListeners.begin();
			while (i != m_pendingJoystickListeners.end()) {
				m_joystickListeners.push_back(*i);
				++i;
			}
			m_pendingJoystickListeners.clear();
		}

		if (!m_pendingJoystickListenersFront.empty()) {
			std::deque<IJoystickListener*>::iterator i = m_pendingJoystickListenersFront.begin();
			while (i != m_pendingJoystickListenersFront.end()) {
				m_joystickListeners.push_front(*i);
				++i;
			}
			m_pendingJoystickListenersFront.clear();
		}

		if (!m_pendingJoystickDeletions.empty()) {
			std::deque<IJoystickListener*>::iterator i = m_pendingJoystickDeletions.begin();
			while (i != m_pendingJoystickDeletions.end()) {
				std::deque<IJoystickListener*>::iterator j = m_joystickListeners.begin();
				while (j != m_joystickListeners.end()) {
					if (*j == *i) {
						m_joystickListeners.erase(j);
						break;
					}
					++j;
				}
				++i;
			}
			m_pendingJoystickDeletions.clear();
		}

		std::deque<IJoystickListener*>::iterator i = m_joystickListeners.begin();
		while (i != m_joystickListeners.end()) {
			switch (evt.getType()) {
			case JoystickEvent::AXIS_MOTION:
				(*i)->axisMotion(evt);
				break;
			default:
				break;
			}
			++i;
		}
	}

	EventSourceType JoystickManager::getEventSourceType() {
		return ES_ENGINE;
	}

}
