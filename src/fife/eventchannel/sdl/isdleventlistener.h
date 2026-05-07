// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ISDLEVENTLISTENER_H
#define FIFE_EVENTCHANNEL_ISDLEVENTLISTENER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
#include "eventchannel/base/ilistener.h"

union SDL_Event;

namespace FIFE
{
    /**  Listener of SDL events.
     * To be able to listen for commands you must make a class which inherits
     * from this class and implements the onSdlEvent function.
     */
    class FIFE_API ISdlEventListener : public IListener
    {
        public:
            /** Called when an SDL event is received from SDL
             * @param evt SDL event
             */
            virtual bool onSdlEvent(SDL_Event& evt) = 0;

            virtual ~ISdlEventListener() = default;
    };

} // namespace FIFE

#endif
