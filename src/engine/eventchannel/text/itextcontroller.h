// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ITEXT_CONTROLLER_H
#define FIFE_EVENTCHANNEL_ITEXT_CONTROLLER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//

namespace FIFE
{
    class ITextListener;

    /**  Controller provides a way to receive events from the system
     * Using this interface, clients can subscribe themselves to receive events
     */
    class ITextController
    {
    public:
        /** Adds a listener to the back of the listener deque
         * Listener will be notified via the corresponding events
         * @param listener listener to add
         */
        virtual void addTextListener(ITextListener* listener) = 0;

        /** Adds a listener to the front of the listener deque
         * Listener will be notified via the corresponding events
         * @param listener listener to add
         */
        virtual void addTextListenerFront(ITextListener* listener) = 0;

        /** Removes an added listener from the controller.
         * Listener will not be notified anymore via the corresponding events
         * @param listener listener to remove
         */
        virtual void removeTextListener(ITextListener* listener) = 0;

        virtual ~ITextController() { }
    };

} // namespace FIFE

#endif