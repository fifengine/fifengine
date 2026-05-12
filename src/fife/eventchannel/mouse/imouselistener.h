// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IMOUSELISTENER_H
#define FIFE_EVENTCHANNEL_IMOUSELISTENER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
#include "eventchannel/base/ilistener.h"

namespace FIFE
{

    class MouseEvent;

    /**  Listener of mouse events.
     * To be able to listen for mouse events you must make a class which inherits
     * from this class and implements its functions.
     */
    class FIFE_API IMouseListener : public IListener
    {
        public:
            /**
             * Called when the mouse has entered into the event source area.
             * @param evt describes the event.
             */
            virtual void mouseEntered(MouseEvent& evt) = 0;

            /**
             * Called when the mouse has exited the event source area.
             * @param evt describes the event.
             */
            virtual void mouseExited(MouseEvent& evt) = 0;

            /**
             * Called when a mouse button has been pressed on the event source area.
             * NOTE: A mouse press is NOT equal to a mouse click.
             * @param evt describes the event.
             */
            virtual void mousePressed(MouseEvent& evt) = 0;

            /**
             * Called when a mouse button has been released on the event source area.
             * @param evt describes the event.
             */
            virtual void mouseReleased(MouseEvent& evt) = 0;

            /**
             * Called when a mouse button is pressed and released (clicked) on
             * the event source area.
             * @param evt describes the event.
             */
            virtual void mouseClicked(MouseEvent& evt) = 0;

            /**
             * Called when the mouse wheel has moved up on the event source area.
             * @param evt describes the event.
             */
            virtual void mouseWheelMovedUp(MouseEvent& evt) = 0;

            /**
             * Called when the mouse wheel has moved down on the event source area.
             * @param evt MouseEvent that describes the event.
             */
            virtual void mouseWheelMovedDown(MouseEvent& evt) = 0;

            /**
             * Called when the mouse wheel has moved right on the event source area.
             * @param evt describes the event.
             */
            virtual void mouseWheelMovedRight(MouseEvent& evt) = 0;

            /**
             * Called when the mouse wheel has moved left on the event source area.
             * @param evt MouseEvent that describes the event.
             */
            virtual void mouseWheelMovedLeft(MouseEvent& evt) = 0;

            /**
             * Called when the mouse has moved in the event source area and no mouse button
             * has been pressed
             * @param evt describes the event.
             */
            virtual void mouseMoved(MouseEvent& evt) = 0;

            /**
             * Called when the mouse has moved and the mouse has previously been
             * pressed on the event source.
             * @param evt describes the event.
             */
            virtual void mouseDragged(MouseEvent& evt) = 0;

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

            virtual ~IMouseListener() = default;

        protected:
            IMouseListener() : m_global(false)
            {
            }

        private:
            bool m_global;
    };

} // namespace FIFE

#endif
