// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IEVENTSOURCE_H
#define FIFE_EVENTCHANNEL_IEVENTSOURCE_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventsourcetypes.h"

namespace FIFE
{

    /**  Representation of event source (a thing sending events)
     */
    class IEventSource
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