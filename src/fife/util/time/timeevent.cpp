// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "timeevent.h"

// Standard C++ library includes
#include <iostream>
#include <utility>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "sdltimecompat.h"
#include "timemanager.h"

namespace FIFE
{

    TimeEvent::TimeEvent(int32_t period) : m_period(period), m_last_updated(TimeManager::instance()->now64())
    {
    }

    TimeEvent::~TimeEvent() = default;

    void TimeEvent::managerUpdateEvent(uint32_t time)
    {
        managerUpdateEvent64(SDLTimeCompat::fromLegacy32Ticks(time));
    }

    void TimeEvent::managerUpdateEvent64(uint64_t time)
    {
        if (m_period < 0) {
            return;
        }

        uint64_t const time_delta = time - m_last_updated;
        if (m_period == 0 || time_delta >= SDLTimeCompat::fromLegacy32Ticks(m_period)) {
            updateEvent(SDLTimeCompat::toUint32Ticks(time_delta));
            m_last_updated = time;
        }
    }

    void TimeEvent::setPeriod(int32_t period)
    {
        m_period = period;
    }

    int32_t TimeEvent::getPeriod() const
    {
        return m_period;
    }

    uint32_t TimeEvent::getLastUpdateTime() const
    {
        return SDLTimeCompat::toUint32Ticks(m_last_updated);
    }

    uint64_t TimeEvent::getLastUpdateTime64() const
    {
        return m_last_updated;
    }

    void TimeEvent::setLastUpdateTime(uint32_t ms)
    {
        m_last_updated = SDLTimeCompat::fromLegacy32Ticks(ms);
    }

    void TimeEvent::setLastUpdateTime64(uint64_t ms)
    {
        m_last_updated = ms;
    }

} // namespace FIFE
