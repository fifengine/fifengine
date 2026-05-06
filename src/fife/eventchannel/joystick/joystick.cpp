// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "joystick.h"

// Standard C++ library includes
#include <iostream>
#include <limits>
#include <string>
#include <utility>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "util/base/exception.h"
#include "util/log/logger.h"
#include "util/math/fife_math.h"

namespace FIFE
{
    static Logger _log(LM_EVTCHANNEL);

    namespace
    {
        uint8_t clampJoystickCount(int value)
        {
            if (value <= 0) {
                return 0;
            }

            if (std::cmp_greater(value, std::numeric_limits<uint8_t>::max())) {
                return std::numeric_limits<uint8_t>::max();
            }

            return static_cast<uint8_t>(value);
        }
    } // namespace

    Joystick::Joystick(int32_t joystickId, int32_t deviceIndex) :
        m_joystickHandle(nullptr),
        m_controllerHandle(nullptr),
        m_instanceId(-1),
        m_joystickId(joystickId),
        m_deviceIndex(deviceIndex)

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

    std::string const & Joystick::getGuid()
    {
        return m_guidStr;
    }

    std::string const & Joystick::getName()
    {
        return m_name;
    }

    void Joystick::open()
    {
        if (m_joystickHandle != nullptr) {
            close();
        }

        m_joystickHandle = SDL_OpenJoystick(m_deviceIndex);
        if (m_joystickHandle != nullptr) {
            m_instanceId = SDL_GetJoystickID(m_joystickHandle);

            char tmp[33];
            SDL_GUID guid = SDL_GetJoystickGUID(m_joystickHandle);
            SDL_GUIDToString(guid, tmp, sizeof(tmp));
            m_guidStr = std::string(tmp);

            openController();
            char const * name = SDL_GetJoystickName(m_joystickHandle);
            if (isController() && (name == nullptr)) {
                name = SDL_GetGamepadName(SDL_GetGamepadFromID(SDL_GetJoystickID(m_joystickHandle)));
            }
            m_name = std::string(name);
        } else {
            throw SDLException(SDL_GetError());
        }
    }

    void Joystick::close()
    {
        closeController();
        if (m_joystickHandle != nullptr) {
            SDL_CloseJoystick(m_joystickHandle);
            m_joystickHandle = nullptr;
        }
        m_instanceId  = -1;
        m_deviceIndex = -1;
    }

    bool Joystick::isConnected() const
    {
        return (m_joystickHandle != nullptr) && SDL_JoystickConnected(m_joystickHandle);
    }

    bool Joystick::isController() const
    {
        return m_controllerHandle != nullptr;
    }

    void Joystick::openController()
    {
        closeController();
        if (m_joystickHandle == nullptr || !SDL_IsGamepad(SDL_GetJoystickID(m_joystickHandle))) {
            return;
        }

        m_controllerHandle = SDL_OpenGamepad(SDL_GetJoystickID(m_joystickHandle));
    }

    void Joystick::closeController()
    {
        if (m_controllerHandle != nullptr) {
            SDL_CloseGamepad(m_controllerHandle);
            m_controllerHandle = nullptr;
        }
    }

    uint8_t Joystick::getNumberOfAxes() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = clampJoystickCount(SDL_GetNumJoystickAxes(m_joystickHandle));
        }
        return number;
    }

    uint8_t Joystick::getNumberOfButtons() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = clampJoystickCount(SDL_GetNumJoystickButtons(m_joystickHandle));
        }
        return number;
    }

    uint8_t Joystick::getNumberOfHats() const
    {
        uint8_t number = 0;
        if (isConnected()) {
            number = clampJoystickCount(SDL_GetNumJoystickHats(m_joystickHandle));
        }
        return number;
    }

    float Joystick::getAxisValue(int8_t axis) const
    {
        if (axis < 0 || !isConnected()) {
            return 0;
        }

        if (!isController()) {
            return convertRange(SDL_GetJoystickAxis(m_joystickHandle, axis));
        }

        auto sdlAxis = static_cast<SDL_GamepadAxis>(axis);
        return convertRange(SDL_GetGamepadAxis(m_controllerHandle, sdlAxis));
    }

    int8_t Joystick::getHatValue(int8_t hat) const
    {
        if (hat < 0 || !isConnected()) {
            return HAT_INVALID;
        }
        return static_cast<int8_t>(SDL_GetJoystickHat(m_joystickHandle, hat));
    }

    bool Joystick::isButtonPressed(int8_t button) const
    {
        if (button < 0 || !isConnected()) {
            return false;
        }
        if (!isController()) {
            return SDL_GetJoystickButton(m_joystickHandle, button) == 1;
        }

        auto sdlButton = static_cast<SDL_GamepadButton>(button);
        return SDL_GetGamepadButton(m_controllerHandle, sdlButton) == 1;
    }

    float Joystick::convertRange(int16_t value) const
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
} // namespace FIFE
