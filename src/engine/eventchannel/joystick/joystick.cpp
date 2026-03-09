// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <iostream>
#include <string>

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

namespace FIFE
{
    static Logger _log(LM_EVTCHANNEL);

    Joystick::Joystick(int32_t joystickId, int32_t deviceIndex) :
        m_joystickHandle(NULL),
        m_controllerHandle(NULL),
        m_instanceId(-1),
        m_joystickId(joystickId),
        m_deviceIndex(deviceIndex),
        m_guidStr(""),
        m_name("")
    {
    }

    Joystick::~Joystick()
    {
        close();
    }

    int32_t Joystick::getInstanceId() const
    {
        return static_cast<int32_t>(m_instanceId);
    }

    int32_t Joystick::getJoystickId() const
    {
        return m_joystickId;
    }

    void Joystick::setDeviceIndex(int32_t deviceIndex)
    {
        m_deviceIndex = deviceIndex;
    }

    int32_t Joystick::getDeviceIndex() const
    {
        return m_deviceIndex;
    }

    const std::string& Joystick::getGuid()
    {
        return m_guidStr;
    }

    const std::string& Joystick::getName()
    {
        return m_name;
    }

    void Joystick::open()
    {
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

    void Joystick::close()
    {
        closeController();
        if (m_joystickHandle) {
            SDL_JoystickClose(m_joystickHandle);
            m_joystickHandle = nullptr;
        }
        m_instanceId  = -1;
        m_deviceIndex = -1;
    }

    bool Joystick::isConnected() const
    {
        return m_joystickHandle && SDL_JoystickGetAttached(m_joystickHandle);
    }

    bool Joystick::isController() const
    {
        return m_controllerHandle != nullptr;
    }

    void Joystick::openController()
    {
        closeController();
        if (!SDL_IsGameController(m_deviceIndex)) {
            return;
        }

        m_controllerHandle = SDL_GameControllerOpen(m_deviceIndex);
    }

    void Joystick::closeController()
    {
        if (m_controllerHandle) {
            SDL_GameControllerClose(m_controllerHandle);
            m_controllerHandle = nullptr;
        }
    }

    uint8_t Joystick::getNumberOfAxes() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = SDL_JoystickNumAxes(m_joystickHandle);
        }
        return number;
    }

    uint8_t Joystick::getNumberOfButtons() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = SDL_JoystickNumButtons(m_joystickHandle);
        }
        return number;
    }

    uint8_t Joystick::getNumberOfHats() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = SDL_JoystickNumHats(m_joystickHandle);
        }
        return number;
    }

    float Joystick::getAxisValue(int8_t axis) const
    {
        if (axis < 0 || !isConnected()) {
            return 0;
        }

        if (!isController()) {
            return convertRange(SDL_JoystickGetAxis(m_joystickHandle, axis));
        }

        SDL_GameControllerAxis sdlAxis = static_cast<SDL_GameControllerAxis>(axis);
        return convertRange(SDL_GameControllerGetAxis(m_controllerHandle, sdlAxis));
    }

    int8_t Joystick::getHatValue(int8_t hat) const
    {
        if (hat < 0 || !isConnected()) {
            return HAT_INVALID;
        }
        return SDL_JoystickGetHat(m_joystickHandle, hat);
    }

    bool Joystick::isButtonPressed(int8_t button) const
    {
        if (button < 0 || !isConnected()) {
            return false;
        }
        if (!isController()) {
            if (SDL_JoystickGetButton(m_joystickHandle, button) == 1) {
                return true;
            }
            return false;
        }

        SDL_GameControllerButton sdlButton = static_cast<SDL_GameControllerButton>(button);
        if (SDL_GameControllerGetButton(m_controllerHandle, sdlButton) == 1) {
            return true;
        }
        return false;
    }

    float Joystick::convertRange(int16_t value) const
    {
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
} // namespace FIFE
