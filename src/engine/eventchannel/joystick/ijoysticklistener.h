// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IJOYSTICKLISTENER_H
#define FIFE_EVENTCHANNEL_IJOYSTICKLISTENER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "eventchannel/base/ilistener.h"

namespace FIFE
{
    class JoystickEvent;

    /** Listener of joystick events.
     * To be able to listen for joystick events you must make a class which inherits
     * from this class and implements its functions.
     */
    class IJoystickListener : public IListener
    {
    public:
        /** Called on a axis motion event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void axisMotion(JoystickEvent& evt) = 0;

        /** Called on a hat motion event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void hatMotion(JoystickEvent& evt) = 0;

        /** Called on a button pressed event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void buttonPressed(JoystickEvent& evt) = 0;

        /** Called on a button released event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void buttonReleased(JoystickEvent& evt) = 0;

        /** Called on a device added event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void deviceAdded(JoystickEvent& evt) = 0;

        /** Called on a device removed event.
         * @param evt JoystickEvent that describes the event.
         */
        virtual void deviceRemoved(JoystickEvent& evt) = 0;

        virtual ~IJoystickListener() { }
    };

} // namespace FIFE

#endif