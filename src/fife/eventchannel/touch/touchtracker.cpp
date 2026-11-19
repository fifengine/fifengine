// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "eventchannel/touch/touchtracker.h"

// Standard C++ library includes
#include <algorithm>

namespace FIFE
{

    namespace
    {
        TouchFinger makeFinger(SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float pressure)
        {
            TouchFinger finger;
            finger.touchId  = touchId;
            finger.fingerId = fingerId;
            finger.x        = x;
            finger.y        = y;
            finger.pressure = pressure;
            return finger;
        }
    } // namespace

    bool TouchTracker::begin(SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float pressure)
    {
        TouchFinger const finger = makeFinger(touchId, fingerId, x, y, pressure);
        auto const result        = m_fingers.insert_or_assign(Key{touchId, fingerId}, finger);
        return result.second;
    }

    bool TouchTracker::update(SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float pressure)
    {
        auto const it = m_fingers.find(Key{touchId, fingerId});
        if (it == m_fingers.end()) {
            return false;
        }

        it->second.x        = x;
        it->second.y        = y;
        it->second.pressure = pressure;
        return true;
    }

    bool TouchTracker::end(SDL_TouchID touchId, SDL_FingerID fingerId)
    {
        return m_fingers.erase(Key{touchId, fingerId}) != 0U;
    }

    void TouchTracker::clear()
    {
        m_fingers.clear();
        m_deviceTypes.clear();
    }

    bool TouchTracker::contains(SDL_TouchID touchId, SDL_FingerID fingerId) const
    {
        return m_fingers.contains(Key{touchId, fingerId});
    }

    std::size_t TouchTracker::fingerCount() const
    {
        return m_fingers.size();
    }

    std::size_t TouchTracker::fingerCount(SDL_TouchID touchId) const
    {
        return static_cast<std::size_t>(std::count_if(m_fingers.begin(), m_fingers.end(), [touchId](auto const & item) {
            return item.first.first == touchId;
        }));
    }

    bool TouchTracker::midpoint(float& x, float& y) const
    {
        if (m_fingers.empty()) {
            return false;
        }

        double sumX = 0.0;
        double sumY = 0.0;
        for (auto const & item : m_fingers) {
            sumX += static_cast<double>(item.second.x);
            sumY += static_cast<double>(item.second.y);
        }

        auto const count = static_cast<double>(m_fingers.size());
        x                = static_cast<float>(sumX / count);
        y                = static_cast<float>(sumY / count);
        return true;
    }

    void TouchTracker::setDeviceType(SDL_TouchID touchId, SDL_TouchDeviceType type)
    {
        m_deviceTypes[touchId] = type;
    }

    bool TouchTracker::isTrackpad(SDL_TouchID touchId) const
    {
        auto const it = m_deviceTypes.find(touchId);
        if (it == m_deviceTypes.end()) {
            return false;
        }

        return (it->second == SDL_TOUCH_DEVICE_INDIRECT_ABSOLUTE) || (it->second == SDL_TOUCH_DEVICE_INDIRECT_RELATIVE);
    }

    void PinchTracker::begin()
    {
        m_active = true;
        m_scale  = 1.0F;
    }

    bool PinchTracker::update(float scale)
    {
        if (!m_active) {
            return false;
        }

        m_scale *= scale;
        return true;
    }

    void PinchTracker::end()
    {
        m_active = false;
        m_scale  = 1.0F;
    }

} // namespace FIFE
