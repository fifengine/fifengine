// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_IDROPLISTENER_H
#define FIFE_EVENTCHANNEL_IDROPLISTENER_H

// Standard C++ library includes
//

// 3rd party library includes
//

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "eventchannel/base/ilistener.h"

namespace FIFE
{
    class DropEvent;

    /**  Listener of drop events.
     * To be able to listen for drop events you must make a class which inherits
     * from this class and implements its functions.
     */
    class IDropListener : public IListener
    {
    public:
        /** Called on a file drop event.
         * @param evt DropEvent that describes the event.
         */
        virtual void fileDropped(DropEvent& evt) = 0;

        virtual ~IDropListener() { }
    };

} // namespace FIFE

#endif