// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ICOMMAND_CONTROLLER_H
#define FIFE_EVENTCHANNEL_ICOMMAND_CONTROLLER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
//
#include "icommandlistener.h"

namespace FIFE
{
    class Command;

    /**  Controller provides a way to receive events from the system
     * Using this interface, clients can subscribe themselves to receive events
     * Also command sending is possible
     */
    class FIFE_API ICommandController
    {
    public:
        /** Adds a listener to the back of the listener deque
         * Listener will be notified via the corresponding events
         * @param listener listener to add
         */
        virtual void addCommandListener(ICommandListener* listener) = 0;

        /** Adds a listener to the front of the listener deque
         * Listener will be notified via the corresponding events
         * @param listener listener to add
         */
        virtual void addCommandListenerFront(ICommandListener* listener) = 0;

        /** Removes an added listener from the controller.
         * Listener will not be notified anymore via the corresponding events
         * @param listener listener to remove
         */
        virtual void removeCommandListener(ICommandListener* listener) = 0;

        /** Use this method to send command to command listeners
         * @param command command to dispatch
         */
        virtual void dispatchCommand(Command& command) = 0;

        virtual ~ICommandController() = default;
    };

} // namespace FIFE

#endif