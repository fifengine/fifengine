// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_ITEXTLISTENER_H
#define FIFE_EVENTCHANNEL_ITEXTLISTENER_H

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
    class TextEvent;

    /**  Listener of text events.
     * To be able to listen for text events you must make a class which inherits
     * from this class and implements its functions.
     */
    class ITextListener : public IListener
    {
    public:
        /** Called on a text input event.
         * @param evt TextEvent that describes the event.
         */
        virtual void textInput(TextEvent& evt) = 0;

        /** Called on a text edit event.
         * @param evt TextEvent that describes the event.
         */
        virtual void textEdit(TextEvent& evt) = 0;

        virtual ~ITextListener() = default;
    };

} // namespace FIFE

#endif