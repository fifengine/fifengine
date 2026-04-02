// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <iostream>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "timeevent.h"
#include "timemanager.h"

namespace FIFE
{

    TimeEvent::TimeEvent(int32_t period) : m_period(period), m_last_updated(TimeManager::instance()->getTime()) { }

    TimeEvent::~TimeEvent() { }

    void TimeEvent::managerUpdateEvent(uint32_t time)
    {
        if (m_period < 0) {
            return;
        }

        const uint32_t time_delta = time - m_last_updated;
        if (m_period == 0 || time_delta >= static_cast<uint32_t>(m_period)) {
            updateEvent(time_delta);
            m_last_updated = time;
        }
    }

    void TimeEvent::setPeriod(int32_t period)
    {
        m_period = period;
    }

    int32_t TimeEvent::getPeriod()
    {
        return m_period;
    }

    uint32_t TimeEvent::getLastUpdateTime()
    {
        return m_last_updated;
    }

    void TimeEvent::setLastUpdateTime(uint32_t ms)
    {
        m_last_updated = ms;
    }

} // namespace FIFE
