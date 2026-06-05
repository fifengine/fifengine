// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_DROPEVENT_H
#define FIFE_EVENTCHANNEL_DROPEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/inputevent.h"
#include "eventchannel/source/ieventsource.h"

namespace FIFE
{

    /**  Class for drop events
     */
    class FIFE_API DropEvent : public InputEvent
    {
        public:
            /** Constructor
             */
            DropEvent() = default;

            /** Destructor.
             */
            ~DropEvent() override = default;

            /** Gets the path for the file that is droped.
             */
            std::string const & getPath() const
            {
                return m_path;
            }

            /** Sets the path for the file that is droped.
             */
            void setPath(std::string const & path)
            {
                m_path = path;
            }

            /** Gets the name of the event.
             */
            std::string const & getName() const override
            {
                static std::string const eventName("DropEvent");
                return eventName;
            }

        private:
            std::string m_path;
    };

} // namespace FIFE

#endif
