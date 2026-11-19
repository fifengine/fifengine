// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ITOUCH_CONTROLLER_H
#define FIFE_EVENTCHANNEL_ITOUCH_CONTROLLER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes

// 3rd party library includes

// FIFE includes

namespace FIFE
{
    class ITouchListener;

    /** Controller for subscribing to raw touch and pinch events. */
    class FIFE_API ITouchController
    {
        public:
            /** Adds a listener to the back of the listener deque. */
            virtual void addTouchListener(ITouchListener* listener) = 0;

            /** Adds a listener to the front of the listener deque. */
            virtual void addTouchListenerFront(ITouchListener* listener) = 0;

            /** Removes an added listener from the controller. */
            virtual void removeTouchListener(ITouchListener* listener) = 0;

            virtual ~ITouchController() = default;
    };

} // namespace FIFE

#endif
