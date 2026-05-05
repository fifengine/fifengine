// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "timer.h"

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "timemanager.h"

namespace FIFE
{

    // Remember TimeEvent(-1) means that even a registered event won't be called
    Timer::Timer() : TimeEvent(-1), m_active(false)
    {
    }

    Timer::~Timer()
    {
        stop();
    }

    void Timer::start()
    {
        if (m_active) {
            return;
        }
        setLastUpdateTime(TimeManager::instance()->getTime());
        TimeManager::instance()->registerEvent(this);
        m_active = true;
    }

    void Timer::stop()
    {
        if (!m_active) {
            return;
        }
        TimeManager::instance()->unregisterEvent(this);
        m_active = false;
    }

    void Timer::setInterval(int32_t msec)
    {
        setPeriod(msec);
        stop();
    }
    void Timer::setCallback(type_callback const & cb)
    {
        m_callback = cb;
    }

    void Timer::updateEvent(uint32_t /*time*/)
    {
        if (!m_active) {
            return;
        }

        if (m_callback) {
            m_callback();
        }
    }
} // namespace FIFE
