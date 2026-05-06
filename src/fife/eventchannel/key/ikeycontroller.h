// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IKEY_CONTROLLER_H
#define FIFE_EVENTCHANNEL_IKEY_CONTROLLER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
//

namespace FIFE
{
    class IKeyListener;

    /**  Controller provides a way to receive events from the system
     * Using this interface, clients can subscribe themselves to receive events
     */
    class /*FIFE_API*/ IKeyController
    {
        public:
            /** Adds a listener to the back of the listener deque
             * Listener will be notified via the corresponding events
             * @param listener listener to add
             */
            virtual void addKeyListener(IKeyListener* listener) = 0;

            /** Adds a listener to the front of the listener deque
             * Listener will be notified via the corresponding events
             * @param listener listener to add
             */
            virtual void addKeyListenerFront(IKeyListener* listener) = 0;

            /** Removes an added listener from the controller.
             * Listener will not be notified anymore via the corresponding events
             * @param listener listener to remove
             */
            virtual void removeKeyListener(IKeyListener* listener) = 0;

            virtual ~IKeyController() = default;
    };

} // namespace FIFE

#endif
