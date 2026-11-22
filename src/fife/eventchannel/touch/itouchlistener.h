// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ITOUCHLISTENER_H
#define FIFE_EVENTCHANNEL_ITOUCHLISTENER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/ilistener.h"

namespace FIFE
{
    class TouchEvent;

    /** Listener for raw touch and pinch events.
     *
     * Every callback has a default no-op body. A Python director may implement
     * only the gestures it cares about without risking a pure-virtual upcall.
     * Mouse listeners still receive the synthesized compatibility events.
     */
    class FIFE_API ITouchListener : public IListener
    {
        public:
            /** Called when a finger touches the device. */
            virtual void touchDown(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called when a finger leaves the device. */
            virtual void touchUp(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called when a finger moves while touching the device. */
            virtual void touchMotion(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called when the system cancels a touch gesture. */
            virtual void touchCanceled(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called when a pinch gesture starts. */
            virtual void pinchBegin(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called for every pinch scale update. */
            virtual void pinchUpdate(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Called when a pinch gesture ends. */
            virtual void pinchEnd(TouchEvent& evt)
            {
                static_cast<void>(evt);
            }

            /** Indicates if this is a global listener.
             * Global listeners also receive events consumed by widgets / GUI.
             */
            virtual bool isGlobalListener() const
            {
                return m_global;
            }

            /** Changes the listener to be global or not. */
            virtual void setGlobalListener(bool global)
            {
                m_global = global;
            }

            ~ITouchListener() override = default;

            /** A listener with all callbacks at their no-op defaults.
             *
             * Public so SWIG can construct the proxy/director object.
             */
            ITouchListener() : m_global(false)
            {
            }

        private:
            bool m_global;
    };

} // namespace FIFE

#endif
