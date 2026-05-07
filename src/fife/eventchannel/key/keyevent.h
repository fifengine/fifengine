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
            virtual ~KeyEvent() = default;

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

            virtual bool isAltPressed() const
            {
                return InputEvent::isAltPressed();
            }
            virtual void setAltPressed(bool pressed)
            {
                InputEvent::setAltPressed(pressed);
            }
            virtual bool isControlPressed() const
            {
                return InputEvent::isControlPressed();
            }
            virtual void setControlPressed(bool pressed)
            {
                InputEvent::setControlPressed(pressed);
            }
            virtual bool isMetaPressed() const
            {
                return InputEvent::isMetaPressed();
            }
            virtual void setMetaPressed(bool pressed)
            {
                InputEvent::setMetaPressed(pressed);
            }
            virtual bool isShiftPressed() const
            {
                return InputEvent::isShiftPressed();
            }
            virtual void setShiftPressed(bool pressed)
            {
                InputEvent::setShiftPressed(pressed);
            }

            virtual void consume()
            {
                InputEvent::consume();
            }
            virtual bool isConsumed() const
            {
                return InputEvent::isConsumed();
            }
            virtual void consumedByWidgets()
            {
                InputEvent::consumedByWidgets();
            }
            virtual bool isConsumedByWidgets() const
            {
                return InputEvent::isConsumedByWidgets();
            }
            virtual IEventSource* getSource() const
            {
                return InputEvent::getSource();
            }
            virtual void setSource(IEventSource* source)
            {
                InputEvent::setSource(source);
            }
            virtual int32_t getTimeStamp() const
            {
                return InputEvent::getTimeStamp();
            }
            virtual void setTimeStamp(int32_t timestamp)
            {
                InputEvent::setTimeStamp(timestamp);
            }

            virtual std::string const & getName() const
            {
                static std::string const eventName("KeyEvent");
                return eventName;
            }
            virtual std::string getDebugString() const
            {
                return InputEvent::getDebugString();
            }

        private:
            KeyEventType m_eventType;
            bool m_isNumericPad;
            Key m_key;
    };

} // namespace FIFE

#endif
