// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_EVENTSOURCETYPES_H
#define FIFE_EVENTCHANNEL_EVENTSOURCETYPES_H

// Standard C++ library includes
#include <cstdint>

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
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
