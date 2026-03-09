// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <algorithm>
#include <cassert>
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/log/logger.h"

#include "timeevent.h"
#include "timemanager.h"

namespace FIFE
{
    static const uint32_t UNDEFINED_TIME_DELTA = 999999;
    static Logger _log(LM_UTIL);

    TimeManager::TimeManager() : m_current_time(0), m_time_delta(UNDEFINED_TIME_DELTA), m_average_frame_time(0) { }

    TimeManager::~TimeManager() = default;

    void TimeManager::update()
    {
        // if first update...
        double avg_multiplier = 0.985;
        if (m_current_time == 0) {
            m_current_time = SDL_GetTicks();
            avg_multiplier = 0;
            m_time_delta   = 0;
        } else {
            m_time_delta   = m_current_time;
            m_current_time = SDL_GetTicks();
            m_time_delta   = m_current_time - m_time_delta;
        }
        m_average_frame_time =
            m_average_frame_time * avg_multiplier + static_cast<double>(m_time_delta) * (1.0 - avg_multiplier);

        // Update live events.
        //
        // It is very important to NOT use iterators (over a vector)
        // here, as an event might add enough events to resize the vector.
        // -> Ugly segfault
        for (size_t i = 0; i < m_events_list.size(); ++i) {
            TimeEvent* event = m_events_list[i];
            if (event != nullptr) {
                event->managerUpdateEvent(m_current_time);
            }
        }

        // Remove dead events
        std::vector<TimeEvent*>::iterator it;
        it = std::remove(m_events_list.begin(), m_events_list.end(), static_cast<TimeEvent*>(nullptr));
        m_events_list.erase(it, m_events_list.end());
    }

    void TimeManager::registerEvent(TimeEvent* event)
    {
        // Register.
        m_events_list.push_back(event);
    }

    void TimeManager::unregisterEvent(TimeEvent* event)
    {
        // Unregister.
        for (size_t i = 0; i < m_events_list.size(); ++i) {
            TimeEvent*& event_i = m_events_list[i];
            if (event_i == event) {
                event_i = nullptr;
                return;
            }
        }
    }

    uint32_t TimeManager::getTime() const
    {
        return m_current_time;
    }

    uint32_t TimeManager::getTimeDelta() const
    {
        return m_time_delta;
    }

    double TimeManager::getAverageFrameTime() const
    {
        return m_average_frame_time;
    }

    void TimeManager::printStatistics() const
    {
        FL_LOG(_log, LMsg("Timers: ") << m_events_list.size());
    }

} // namespace FIFE
