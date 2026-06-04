// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_KEYEVENT_H
#define FIFE_EVENTCHANNEL_KEYEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/inputevent.h"
#include "eventchannel/source/ieventsource.h"
#include "key.h"

namespace FIFE
{

    /**  Class for key events
     */
    class FIFE_API KeyEvent : public InputEvent
    {
        public:
            enum KeyEventType : int8_t
            {
                UNKNOWN  = -1,
                PRESSED  = 0,
                RELEASED = 1
            };

            /** Constructor
             */
            KeyEvent() : m_eventType(UNKNOWN), m_isNumericPad(false)
            {
            }

            /** Destructor.
             */
            virtual ~KeyEvent() override = default;

            KeyEventType getType() const
            {
                return m_eventType;
            }
            void setType(KeyEventType type)
            {
                m_eventType = type;
            }

            bool isNumericPad() const
            {
                return m_isNumericPad;
            }
            void setNumericPad(bool ispad)
            {
                m_isNumericPad = ispad;
            }

            Key const & getKey() const
            {
                return m_key;
            }
            void setKey(Key const & key)
            {
                m_key = key;
            }

            virtual std::string const & getName() const override
            {
                static std::string const eventName("KeyEvent");
                return eventName;
            }

        private:
            KeyEventType m_eventType;
            bool m_isNumericPad;
            Key m_key;
    };

} // namespace FIFE

#endif
