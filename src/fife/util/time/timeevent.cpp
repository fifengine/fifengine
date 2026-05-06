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
#include "timemanager.h"

namespace FIFE
{

    TimeEvent::TimeEvent(int32_t period) : m_period(period), m_last_updated(TimeManager::instance()->getTime())
    {
    }

    TimeEvent::~TimeEvent() = default;

    void TimeEvent::managerUpdateEvent(uint32_t time)
    {
        if (m_period < 0) {
            return;
        }

        uint32_t const time_delta = time - m_last_updated;
        if (m_period == 0 || std::cmp_greater_equal(time_delta, m_period)) {
            updateEvent(time_delta);
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
        return m_last_updated;
    }

    void TimeEvent::setLastUpdateTime(uint32_t ms)
    {
        m_last_updated = ms;
    }

} // namespace FIFE
