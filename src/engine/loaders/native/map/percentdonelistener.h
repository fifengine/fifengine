// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PERCENT_DONE_LISTENER_H
#define FIFE_PERCENT_DONE_LISTENER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes
#include "util/base/fife_stdint.h"

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE
{
    class PercentDoneListener
    {
    public:
        virtual ~PercentDoneListener();
        virtual void OnEvent(unsigned int percentDone) = 0;
    };

    class PercentDoneCallback
    {
    public:
        PercentDoneCallback();
        virtual ~PercentDoneCallback();

        void setTotalNumberOfElements(unsigned int totalElements);
        void setPercentDoneInterval(unsigned int percent);
        void incrementCount();
        void reset();
        void addListener(PercentDoneListener* listener);
        void removeListener(PercentDoneListener* listener);

    private:
        void fireEvent(uint32_t percent);

    private:
        uint32_t m_totalElements;
        uint32_t m_percent;
        uint32_t m_numberOfEvents;
        uint32_t m_count;

        typedef std::vector<PercentDoneListener*> ListenerContainer;
        ListenerContainer m_listeners;
    };
} // namespace FIFE

#endif