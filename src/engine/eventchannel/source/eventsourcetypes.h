// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_EVENTSOURCETYPES_H
#define FIFE_EVENTCHANNEL_EVENTSOURCETYPES_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>

// 3rd party library includes
//

// FIFE includes
//

namespace FIFE
{

    /**  Types for different event sources
     */
    enum EventSourceType : std::uint8_t
    {
        ES_ENGINE          = 1,
        ES_FIFECHAN_WIDGET = 2
    };

} // namespace FIFE

#endif
