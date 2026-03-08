// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IKEYFILTER_H
#define FIFE_EVENTCHANNEL_IKEYFILTER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "keyevent.h"

namespace FIFE
{
    /**  Controller provides a way to receive events from the system
     * Using this interface, clients can subscribe themselves to receive events
     */
    class IKeyFilter
    {
    public:
        /** Check whether a keyevent should be filtered out.
         * @param event They key event.
         */
        virtual bool isFiltered(const KeyEvent& event) = 0;

        virtual ~IKeyFilter() { }
    };

} // namespace FIFE

#endif