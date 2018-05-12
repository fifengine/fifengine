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

		// add already connected joysticks / controllers
		for (int32_t i = 0; i < SDL_NumJoysticks(); ++i) {
			addJoystick(i);
		}
		// enable joystick and gamecontroller events
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_GameControllerEventState(SDL_ENABLE);
	}

	JoystickManager::~JoystickManager() {
		for (std::vector<Joystick*>::iterator it = m_joysticks.begin(); it != m_joysticks.end(); ++it) {
			delete *it;
		}

		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	}

	Joystick* JoystickManager::addJoystick(int32_t deviceIndex) {
		Joystick* joystick = NULL;
		for (std::vector<Joystick*>::iterator it = m_activeJoysticks.begin(); it != m_activeJoysticks.end(); ++it) {
			if ((*it)->getDeviceIndex() == deviceIndex) {
				return joystick;
			}
		}
		std::string guidStr = getGuidString(deviceIndex);
		for (std::vector<Joystick*>::iterator it = m_joysticks.begin(); it != m_joysticks.end(); ++it) {
			if (!(*it)->isConnected() && (*it)->getGuid() == guidStr) {
				joystick = *it;
				break;
			}
		}
		if (!joystick) {
			joystick = new Joystick(m_joysticks.size(), deviceIndex);
			m_joysticks.push_back(joystick);
		} else {
			joystick->setDeviceIndex(deviceIndex);
		}
		joystick->open();
		addControllerGuid(joystick);
		m_joystickIndices.insert(std::pair<int32_t, uint32_t>(joystick->getInstanceId(), joystick->getJoystickId()));
		m_activeJoysticks.push_back(joystick);
		return joystick;
	}

	Joystick* JoystickManager::getJoystick(int32_t instanceId) {
		Joystick* joy = NULL;
		std::map<int32_t, uint32_t>::iterator it = m_joystickIndices.find(instanceId);
		if (it != m_joystickIndices.end()) {
			joy = m_joysticks[it->second];
		}
		return joy;
	}

	void JoystickManager::removeJoystick(Joystick* joystick) {
		std::vector<Joystick*>::iterator it = std::find(m_activeJoysticks.begin(), m_activeJoysticks.end(), joystick);
		if (it != m_activeJoysticks.end()) {
			m_joystickIndices.erase((*it)->getInstanceId());
			removeControllerGuid(*it);
			(*it)->close();
			m_activeJoysticks.erase(it);
		}
	}

	uint8_t JoystickManager::getJoystickCount() const {
		return static_cast<uint8_t>(m_activeJoysticks.size());
	}

	void JoystickManager::loadMapping(const std::string& file) {
		m_mappingLoader.load(file);
		// check if one of the joysticks can now be opened as gamecontroller
		for (std::vector<Joystick*>::iterator it = m_activeJoysticks.begin(); it != m_activeJoysticks.end(); ++it) {
			if (!(*it)->isController()) {
				(*it)->openController();
				addControllerGuid(*it);
			}
		}
	}

	void JoystickManager::saveMapping(const std::string guid, const std::string& file) {
		std::string stringMapping = getStringMapping(guid);
		m_mappingSaver.save(stringMapping, file);
	}

	void JoystickManager::saveMappings(const std::string& file) {
		std::string stringMappings;
		std::map<std::string, uint8_t>::iterator it = m_gamepadGuids.begin();
		for (; it != m_gamepadGuids.end(); ++it) {
			stringMappings += getStringMapping(it->first);
		}
		m_mappingSaver.save(stringMappings, file);
	}

	std::string JoystickManager::getStringMapping(const std::string& guid) {
		SDL_JoystickGUID realGuid = SDL_JoystickGetGUIDFromString(guid.c_str());
		char* mapping = SDL_GameControllerMappingForGUID(realGuid);
		if (!mapping) {
			throw SDLException(SDL_GetError());
			return std::string();
		}

		std::string stringMapping(mapping);
		SDL_free(mapping);
		// add missing platform if needed
		if (stringMapping.find_last_of(',') != stringMapping.length() - 1) {
			stringMapping += ",";
		}
		std::size_t platPos = stringMapping.find("platform:");
		if (platPos == std::string::npos) {
			stringMapping += "platform:" + std::string(SDL_GetPlatform()) + ",\n";
		}
		return stringMapping;
	}

	void JoystickManager::setStringMapping(const std::string& mapping) {
		int32_t result = SDL_GameControllerAddMapping(mapping.c_str());
		if (result == 1) {
			// check if one of the joysticks can now be opened as gamecontroller
			for (std::vector<Joystick*>::iterator it = m_activeJoysticks.begin(); it != m_activeJoysticks.end(); ++it) {
				if (!(*it)->isController()) {
					(*it)->openController();
					addControllerGuid(*it);
				}
			}
		} else if (result == -1) {
			throw SDLException(SDL_GetError());
		}
	}

	void JoystickManager::addJoystickListener(IJoystickListener* listener) {
		m_joystickListeners.push_back(listener);
	}

	void JoystickManager::addJoystickListenerFront(IJoystickListener* listener) {
		m_joystickListeners.push_front(listener);
	}

	void JoystickManager::removeJoystickListener(IJoystickListener* listener) {
		if (listener->isActive()) {
			listener->setActive(false);
			for (std::deque<IJoystickListener*>::iterator it = m_joystickListeners.begin(); it != m_joystickListeners.end(); ++it) {
				if (*it = listener) {
					m_joystickListeners.erase(it);
					break;
				}
			}
		}
	}

	void JoystickManager::processJoystickEvent(SDL_Event event) {
		bool dispatch = true;
		JoystickEvent joyevt;
		joyevt.setSource(this);

		if (event.type == SDL_JOYAXISMOTION) {
			joyevt.setType(JoystickEvent::AXIS_MOTION);
			joyevt.setInstanceId(event.jaxis.which);
			joyevt.setAxis(event.jaxis.axis);
			joyevt.setAxisValue(convertRange(event.jaxis.value));
		} else if (event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP) {
			joyevt.setType(event.type == SDL_JOYBUTTONDOWN ? JoystickEvent::BUTTON_PRESSED : JoystickEvent::BUTTON_RELEASED);
			joyevt.setInstanceId(event.jbutton.which);
			joyevt.setButton(event.jbutton.button);
		} else if (event.type == SDL_JOYHATMOTION) {
			joyevt.setType(JoystickEvent::HAT_MOTION);
			joyevt.setInstanceId(event.jhat.which);
			joyevt.setHat(event.jhat.hat);
			joyevt.setHatValue(event.jhat.value);
		} else if (event.type == SDL_JOYDEVICEADDED) {
			joyevt.setType(JoystickEvent::DEVICE_ADDED);
			// Note: In this case it's the device index, instead of instance id
			Joystick* joy = addJoystick(event.jdevice.which);
			if (joy) {
				joyevt.setInstanceId(joy->getInstanceId());
			} else {
				dispatch = false;
			}
		} else if (event.type == SDL_JOYDEVICEREMOVED) {
			joyevt.setType(JoystickEvent::DEVICE_REMOVED);
			joyevt.setInstanceId(event.jdevice.which);
		} else {
			dispatch = false;
		}
		// Dispatch only if it's not a controller, SDL sends events twice.
		// Only exception for added and removed events.
		Joystick* joy = getJoystick(joyevt.getInstanceId());
		dispatch = dispatch && (!joy->isController() || (event.type == SDL_JOYDEVICEREMOVED || event.type == SDL_JOYDEVICEADDED));
		if (dispatch) {
			joyevt.setController(joy->isController());
			dispatchJoystickEvent(joyevt);
		}
		// Remove it after event dispatch.
		if (event.type == SDL_JOYDEVICEREMOVED) {
			removeJoystick(joy);
		}
	}

	void JoystickManager::processControllerEvent(SDL_Event event) {
		bool dispatch = true;
		JoystickEvent joyevt;
		joyevt.setSource(this);
		joyevt.setController(true);

		if (event.type == SDL_CONTROLLERAXISMOTION) {
			joyevt.setType(JoystickEvent::AXIS_MOTION);
			joyevt.setInstanceId(event.caxis.which);
			joyevt.setAxis(event.caxis.axis);
			joyevt.setAxisValue(convertRange(event.caxis.value));
		} else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
			joyevt.setType(event.type == SDL_CONTROLLERBUTTONDOWN ? JoystickEvent::BUTTON_PRESSED : JoystickEvent::BUTTON_RELEASED);
			joyevt.setInstanceId(event.cbutton.which);
			joyevt.setButton(event.cbutton.button);
		} else {
			dispatch = false;
		}

		if (dispatch) {
			dispatchJoystickEvent(joyevt);
		}
	}


	void JoystickManager::dispatchJoystickEvent(JoystickEvent& evt) {
		std::deque<IJoystickListener*> listeners = m_joystickListeners;
		std::deque<IJoystickListener*>::iterator i = listeners.begin();
		for (; i != listeners.end(); ++i) {
			if (!(*i)->isActive()) continue;
			switch (evt.getType()) {
				case JoystickEvent::AXIS_MOTION:
					(*i)->axisMotion(evt);
					break;
				case JoystickEvent::HAT_MOTION:
					(*i)->hatMotion(evt);
					break;
				case JoystickEvent::BUTTON_PRESSED:
					(*i)->buttonPressed(evt);
					break;
				case JoystickEvent::BUTTON_RELEASED:
					(*i)->buttonReleased(evt);
					break;
				case JoystickEvent::DEVICE_ADDED:
					(*i)->deviceAdded(evt);
					break;
				case JoystickEvent::DEVICE_REMOVED:
					(*i)->deviceRemoved(evt);
					break;
				default:
					break;
			}
			if (evt.isConsumed()) {
				break;
			}
		}
	}

	EventSourceType JoystickManager::getEventSourceType() {
		return ES_ENGINE;
	}

	std::string JoystickManager::getGuidString(int32_t deviceIndex) {
		char tmp[33];
		SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID(deviceIndex);
		SDL_JoystickGetGUIDString(guid, tmp, sizeof(tmp));
		std::string guidString(tmp);
		return guidString;
	}

	float JoystickManager::convertRange(int16_t value) {
		float range = static_cast<float>(value) / 32768.0f;
		if (Mathf::FAbs(range) < 0.01f) {
			return 0.0f;
		}
		if (range < -0.99f) {
			return -1.0f;
		} else if (range > 0.99f) {
			return 1.0f;
		}
		return range;
	}

	void JoystickManager::addControllerGuid(Joystick* joystick) {
		if (!joystick->isController()) {
			return;
		}
		std::pair<std::map<std::string, uint8_t>::iterator, bool> ret;
		ret = m_gamepadGuids.insert(std::pair<std::string, uint8_t>(joystick->getGuid(), 1));
		if (ret.second == false) {
			++ret.first->second;
		}
	}

	void JoystickManager::removeControllerGuid(Joystick* joystick) {
		if (!joystick->isController()) {
			return;
		}
		std::map<std::string, uint8_t>::iterator it = m_gamepadGuids.find(joystick->getGuid());
		if (it != m_gamepadGuids.end()) {
			--it->second;
		}
	}
}
