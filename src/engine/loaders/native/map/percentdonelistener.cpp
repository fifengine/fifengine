// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <iostream>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#include "percentdonelistener.h"

namespace FIFE
{
    const uint32_t minPercent = 0;
    const uint32_t maxPercent = 100;

    PercentDoneListener::~PercentDoneListener() = default;

    PercentDoneCallback::PercentDoneCallback() : m_totalElements(0), m_percent(1), m_numberOfEvents(0), m_count(0) { }

    PercentDoneCallback::~PercentDoneCallback() = default;

    void PercentDoneCallback::setTotalNumberOfElements(unsigned int totalElements)
    {
        m_totalElements = totalElements;
    }

    void PercentDoneCallback::setPercentDoneInterval(unsigned int percent)
    {
        m_percent = percent;
    }

    void PercentDoneCallback::incrementCount()
    {

        if (m_count == minPercent) {
            // go ahead and fire event just to tell clients we are starting
            fireEvent(minPercent);
        }

        // increment count
        ++m_count;

        // only go through the effort of figuring out percent done if we have listeners
        // and we have a total number of elements greater than 0
        if (!m_listeners.empty() && m_totalElements > 0) {
            if (m_count >= m_totalElements) {
                fireEvent(maxPercent);
            } else {
                // calculate percent done
                auto percentDone =
                    static_cast<uint32_t>((static_cast<float>(m_count) / m_totalElements) * maxPercent);

                if ((percentDone % m_percent) == 0 && (percentDone != m_percent * m_numberOfEvents)) {
                    // keep track of how many times event has occurred
                    ++m_numberOfEvents;

                    // alert listeners of event
                    fireEvent(m_percent * m_numberOfEvents);
                }
            }
        }
    }

    void PercentDoneCallback::reset()
    {
        m_totalElements  = 0;
        m_count          = 0;
        m_numberOfEvents = 0;

        // send event to alert of the reset
        fireEvent(minPercent);
    }

    void PercentDoneCallback::addListener(PercentDoneListener* listener)
    {
        if (listener != nullptr) {
            m_listeners.push_back(listener);
        }
    }

    void PercentDoneCallback::removeListener(PercentDoneListener* listener)
    {
        auto iter = m_listeners.begin();
        for (; iter != m_listeners.end(); ++iter) {
            if (*iter == listener) {
                m_listeners.erase(iter);
                break;
            }
        }
    }

    void PercentDoneCallback::fireEvent(uint32_t percent)
    {
        auto iter = m_listeners.begin();
        for (; iter != m_listeners.end(); ++iter) {
            (*iter)->OnEvent(percent);
        }
    }
} // namespace FIFE