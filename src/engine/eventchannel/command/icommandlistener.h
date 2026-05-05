// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ICOMMANDLISTENER_H
#define FIFE_EVENTCHANNEL_ICOMMANDLISTENER_H

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
    class Command;

    /**  Listener of command events.
     * To be able to listen for commands you must make a class which inherits
     * from this class and implements the onCommand function.
     */
    class /*FIFE_API*/ ICommandListener : public IListener
    {
        public:
            /** Called when a command is received from a command source
             * @param command command coming from command source
             */
            virtual void onCommand(Command& command) = 0;

            virtual ~ICommandListener() = default;
    };

} // namespace FIFE

#endif
