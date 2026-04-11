// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IKEYFILTER_H
#define FIFE_EVENTCHANNEL_IKEYFILTER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
//
#include "keyevent.h"

namespace FIFE
{
    /**  Controller provides a way to receive events from the system
     * Using this interface, clients can subscribe themselves to receive events
     */
    class FIFE_API IKeyFilter
    {
    public:
        /** Check whether a keyevent should be filtered out.
         * @param event They key event.
         */
        virtual bool isFiltered(const KeyEvent& event) = 0;

        virtual ~IKeyFilter() = default;
    };

} // namespace FIFE

#endif