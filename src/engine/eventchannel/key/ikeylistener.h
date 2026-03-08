// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IKEYLISTENER_H
#define FIFE_EVENTCHANNEL_IKEYLISTENER_H

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
    class KeyEvent;

    /**  Listener of key events.
     * To be able to listen for key events you must make a class which inherits
     * from this class and implements its functions.
     */
    class IKeyListener : public IListener
    {
    public:
        /** Called if a key is pressed
         * If a key is held down the multiple key presses are generated.
         * @param evt KeyEvent that describes the event.
         */
        virtual void keyPressed(KeyEvent& evt) = 0;

        /** Called if a key is released
         * @param evt KeyEvent that describes the event.
         */
        virtual void keyReleased(KeyEvent& evt) = 0;

        /** Indicates if this is a global listener
         * Global listener can also receive events that are consumed by widgets / gui.
         * Default value is false.
         */
        virtual bool isGlobalListener() const
        {
            return m_global;
        }

        /** Changes the behaviour of the listener to be global or not.
         * Global listener can also receive events that are consumed by widgets / gui.
         * @param global The value to change the behaviour of this listener.
         */
        virtual void setGlobalListener(bool global)
        {
            m_global = global;
        }

        virtual ~IKeyListener() { }

    protected:
        IKeyListener()
        {
            m_global = false;
        }

    private:
        bool m_global;
    };

} // namespace FIFE

#endif