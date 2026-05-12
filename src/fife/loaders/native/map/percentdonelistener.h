// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PERCENT_DONE_LISTENER_H
#define FIFE_PERCENT_DONE_LISTENER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <vector>

// 3rd party library includes
#include "util/base/fife_stdint.h"

// FIFE includes

namespace FIFE
{
    class FIFE_API PercentDoneListener
    {
        public:
            virtual ~PercentDoneListener();
            virtual void OnEvent(unsigned int percentDone) = 0;
    };

    class FIFE_API PercentDoneCallback
    {
        public:
            PercentDoneCallback();
            virtual ~PercentDoneCallback();

            void setTotalNumberOfElements(unsigned int totalElements);
            void setPercentDoneInterval(unsigned int percent);
            void incrementCount();
            void reset();
            void addListener(PercentDoneListener* listener);
            void removeListener(PercentDoneListener const * listener);

        private:
            void fireEvent(uint32_t percent);

            uint32_t m_totalElements;
            uint32_t m_percent;
            uint32_t m_numberOfEvents;
            uint32_t m_count;

            using ListenerContainer = std::vector<PercentDoneListener*>;
            ListenerContainer m_listeners;
    };
} // namespace FIFE

#endif
