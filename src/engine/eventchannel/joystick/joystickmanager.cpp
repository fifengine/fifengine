// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "joystickmanager.h"

// Standard C++ library includes
#include <algorithm>
#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes// #include "loaders/native/input/controllermappingloader.h"
// #include "savers/native/input/controllermappingsaver.h"
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"

namespace FIFE
{
    static Logger _log(LM_EVTCHANNEL);

    JoystickManager::JoystickManager()
    {
        // init joystick and controller systems
        if (!SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD)) {
            throw SDLException(SDL_GetError());
        }
        // create loader/saver for controller mappings
        m_mappingLoader = ControllerMappingLoader();
        m_mappingSaver  = ControllerMappingSaver();

        // add already connected joysticks / controllers
        int count;
        SDL_JoystickID* joysticks = SDL_GetJoysticks(&count);
        for (int32_t i = 0; i < count; ++i) {
            addJoystick(i);
        }
        SDL_free(joysticks);
        // enable joystick and gamecontroller events
        SDL_SetJoystickEventsEnabled(true);
        SDL_SetGamepadEventsEnabled(true);
    }

    JoystickManager::~JoystickManager()
    {
        for (auto& m_joystick : m_joysticks) {
            delete m_joystick;
        }

        SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD);
    }

    Joystick* JoystickManager::addJoystick(int32_t deviceIndex)
    {
        Joystick* joystick = nullptr;
        if (std::ranges::any_of(m_activeJoysticks, [deviceIndex](Joystick const * j) {
                return j->getDeviceIndex() == deviceIndex;
            })) {
            return joystick;
        }
        std::string guidStr = getGuidString(deviceIndex);

        auto it = std::ranges::find_if(m_joysticks, [&guidStr](Joystick* j) {
            return !j->isConnected() && j->getGuid() == guidStr;
        });
        if (it != m_joysticks.end()) {
            joystick = *it;
        }
        if (joystick == nullptr) {
            if (m_joysticks.size() > static_cast<size_t>(std::numeric_limits<int32_t>::max())) {
                throw InvalidFormat("Too many joystick instances registered");
            }
            joystick = new Joystick(static_cast<int32_t>(m_joysticks.size()), deviceIndex);
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

    Joystick* JoystickManager::getJoystick(int32_t instanceId)
    {
        Joystick* joy = nullptr;
        auto it       = m_joystickIndices.find(instanceId);
        if (it != m_joystickIndices.end()) {
            joy = m_joysticks[it->second];
        }
        return joy;
    }

    void JoystickManager::removeJoystick(Joystick* joystick)
    {
        auto it = std::ranges::find(m_activeJoysticks, joystick);
        if (it != m_activeJoysticks.end()) {
            m_joystickIndices.erase((*it)->getInstanceId());
            removeControllerGuid(*it);
            (*it)->close();
            m_activeJoysticks.erase(it);
        }
    }

    uint8_t JoystickManager::getJoystickCount() const
    {
        return static_cast<uint8_t>(m_activeJoysticks.size());
    }

    void JoystickManager::loadMapping(std::string const & file)
    {
        m_mappingLoader.load(file);
        // check if one of the joysticks can now be opened as gamecontroller
        for (auto& m_activeJoystick : m_activeJoysticks) {
            if (!m_activeJoystick->isController()) {
                m_activeJoystick->openController();
                addControllerGuid(m_activeJoystick);
            }
        }
    }

    void JoystickManager::saveMapping(std::string const & guid, std::string const & file)
    {
        std::string const stringMapping = getStringMapping(guid);
        m_mappingSaver.save(stringMapping, file);
    }

    void JoystickManager::saveMappings(std::string const & file)
    {
        std::string stringMappings;
        auto it = m_gamepadGuids.begin();
        for (; it != m_gamepadGuids.end(); ++it) {
            stringMappings += getStringMapping(it->first);
        }
        m_mappingSaver.save(stringMappings, file);
    }

    std::string JoystickManager::getStringMapping(std::string const & guid)
    {
        SDL_GUID const realGuid = SDL_StringToGUID(guid.c_str());
        char* mapping           = SDL_GetGamepadMappingForGUID(realGuid);
        if (mapping == nullptr) {
            throw SDLException(SDL_GetError());
        }

        std::string stringMapping(mapping);
        SDL_free(mapping);
        // add missing platform if needed
        if (stringMapping.find_last_of(',') != stringMapping.length() - 1) {
            stringMapping += ",";
        }
        std::size_t const platPos = stringMapping.find("platform:");
        if (platPos == std::string::npos) {
            stringMapping += "platform:" + std::string(SDL_GetPlatform()) + ",\n";
        }
        return stringMapping;
    }

    void JoystickManager::setStringMapping(std::string const & mapping)
    {
        int32_t const result = SDL_AddGamepadMapping(mapping.c_str());
        if (result == 1) {
            // check if one of the joysticks can now be opened as gamecontroller
            for (auto& m_activeJoystick : m_activeJoysticks) {
                if (!m_activeJoystick->isController()) {
                    m_activeJoystick->openController();
                    addControllerGuid(m_activeJoystick);
                }
            }
        } else if (result == -1) {
            throw SDLException(SDL_GetError());
        }
    }

    void JoystickManager::addJoystickListener(IJoystickListener* listener)
    {
        m_joystickListeners.push_back(listener);
    }

    void JoystickManager::addJoystickListenerFront(IJoystickListener* listener)
    {
        m_joystickListeners.push_front(listener);
    }

    void JoystickManager::removeJoystickListener(IJoystickListener* listener)
    {
        if (listener->isActive()) {
            listener->setActive(false);
            auto it = std::ranges::find(m_joystickListeners, listener);
            if (it != m_joystickListeners.end()) {
                m_joystickListeners.erase(it);
            }
        }
    }

    void JoystickManager::processJoystickEvent(SDL_Event event)
    {
        bool dispatch = true;
        JoystickEvent joyevt;
        joyevt.setSource(this);

        if (event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION) {
            joyevt.setType(JoystickEvent::AXIS_MOTION);
            joyevt.setInstanceId(event.jaxis.which);
            joyevt.setAxis(event.jaxis.axis);
            joyevt.setAxisValue(convertRange(event.jaxis.value));
        } else if (event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN || event.type == SDL_EVENT_JOYSTICK_BUTTON_UP) {
            joyevt.setType(
                event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN ? JoystickEvent::BUTTON_PRESSED :
                                                               JoystickEvent::BUTTON_RELEASED);
            joyevt.setInstanceId(event.jbutton.which);
            joyevt.setButton(event.jbutton.button);
        } else if (event.type == SDL_EVENT_JOYSTICK_HAT_MOTION) {
            joyevt.setType(JoystickEvent::HAT_MOTION);
            joyevt.setInstanceId(event.jhat.which);
            joyevt.setHat(event.jhat.hat);
            joyevt.setHatValue(event.jhat.value);
        } else if (event.type == SDL_EVENT_JOYSTICK_ADDED) {
            joyevt.setType(JoystickEvent::DEVICE_ADDED);
            // Note: In this case it's the device index, instead of instance id
            Joystick* joy = addJoystick(event.jdevice.which);
            if (joy != nullptr) {
                joyevt.setInstanceId(joy->getInstanceId());
            } else {
                dispatch = false;
            }
        } else if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
            joyevt.setType(JoystickEvent::DEVICE_REMOVED);
            joyevt.setInstanceId(event.jdevice.which);
        } else {
            dispatch = false;
        }
        // Dispatch only if it's not a controller, SDL sends events twice.
        // Only exception for added and removed events.
        Joystick* joy = getJoystick(joyevt.getInstanceId());
        dispatch = dispatch && (!joy->isController() ||
                                (event.type == SDL_EVENT_JOYSTICK_REMOVED || event.type == SDL_EVENT_JOYSTICK_ADDED));
        if (dispatch) {
            joyevt.setController(joy->isController());
            dispatchJoystickEvent(joyevt);
        }
        // Remove it after event dispatch.
        if (event.type == SDL_EVENT_JOYSTICK_REMOVED) {
            removeJoystick(joy);
        }
    }

    void JoystickManager::processControllerEvent(SDL_Event event)
    {
        bool dispatch = true;
        JoystickEvent joyevt;
        joyevt.setSource(this);
        joyevt.setController(true);

        if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
            joyevt.setType(JoystickEvent::AXIS_MOTION);
            joyevt.setInstanceId(event.gaxis.which);
            joyevt.setAxis(event.gaxis.axis);
            joyevt.setAxisValue(convertRange(event.gaxis.value));
        } else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN || event.type == SDL_EVENT_GAMEPAD_BUTTON_UP) {
            joyevt.setType(
                event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN ? JoystickEvent::BUTTON_PRESSED :
                                                              JoystickEvent::BUTTON_RELEASED);
            joyevt.setInstanceId(event.gbutton.which);
            joyevt.setButton(event.gbutton.button);
        } else {
            dispatch = false;
        }

        if (dispatch) {
            dispatchJoystickEvent(joyevt);
        }
    }

    void JoystickManager::dispatchJoystickEvent(JoystickEvent& evt)
    {
        std::deque<IJoystickListener*> listeners = m_joystickListeners;
        auto i                                   = listeners.begin();
        for (; i != listeners.end(); ++i) {
            if (!(*i)->isActive()) {
                continue;
            }
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
            case JoystickEvent::UNKNOWN_EVENT:
            default:
                break;
            }
            if (evt.isConsumed()) {
                break;
            }
        }
    }

    EventSourceType JoystickManager::getEventSourceType()
    {
        return ES_ENGINE;
    }

    std::string JoystickManager::getGuidString(int32_t deviceIndex)
    {
        char tmp[33];
        SDL_Joystick* joy = SDL_OpenJoystick(deviceIndex);
        if (joy) {
            SDL_GUID guid = SDL_GetJoystickGUID(joy);
            SDL_GUIDToString(guid, &tmp[0], sizeof(tmp));
            SDL_CloseJoystick(joy);
        } else {
            tmp[0] = '\0';
        }
        std::string guidString(&tmp[0]);
        return guidString;
    }

    float JoystickManager::convertRange(int16_t value)
    {
        float const range = static_cast<float>(value) / 32768.0F;
        if (Mathf::FAbs(range) < 0.01F) {
            return 0.0F;
        }
        if (range < -0.99F) {
            return -1.0F;
        }
        if (range > 0.99F) {
            return 1.0F;
        }
        return range;
    }

    void JoystickManager::addControllerGuid(Joystick* joystick)
    {
        if (!joystick->isController()) {
            return;
        }
        std::pair<std::map<std::string, uint8_t>::iterator, bool> ret;
        ret = m_gamepadGuids.insert(std::pair<std::string, uint8_t>(joystick->getGuid(), 1));
        if (!ret.second) {
            ++ret.first->second;
        }
    }

    void JoystickManager::removeControllerGuid(Joystick* joystick)
    {
        if (!joystick->isController()) {
            return;
        }
        auto it = m_gamepadGuids.find(joystick->getGuid());
        if (it != m_gamepadGuids.end()) {
            --it->second;
        }
    }
} // namespace FIFE
