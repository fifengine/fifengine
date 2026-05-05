// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IDROPLISTENER_H
#define FIFE_EVENTCHANNEL_IDROPLISTENER_H

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
    class DropEvent;

    /**  Listener of drop events.
     * To be able to listen for drop events you must make a class which inherits
     * from this class and implements its functions.
     */
    class /*FIFE_API*/ IDropListener : public IListener
    {
        public:
            /** Called on a file drop event.
             * @param evt DropEvent that describes the event.
             */
            virtual void fileDropped(DropEvent& evt) = 0;

            virtual ~IDropListener() = default;
    };

} // namespace FIFE

#endif
