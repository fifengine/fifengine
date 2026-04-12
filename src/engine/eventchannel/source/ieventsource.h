// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IEVENTSOURCE_H
#define FIFE_EVENTCHANNEL_IEVENTSOURCE_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
//
#include "eventsourcetypes.h"

namespace FIFE
{

    /**  Representation of event source (a thing sending events)
     */
    class /*FIFE_API*/ IEventSource
    {
    public:
        /** Gets the source type of this event
         * @return source type of this event
         */
        virtual EventSourceType getEventSourceType() = 0;

        virtual ~IEventSource() = default;
    };

} // namespace FIFE

#endif