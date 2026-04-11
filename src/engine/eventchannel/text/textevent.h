// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_TEXTEVENT_H
#define FIFE_EVENTCHANNEL_TEXTEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
//
#include "eventchannel/base/inputevent.h"
#include "eventchannel/source/ieventsource.h"

#include "text.h"

namespace FIFE
{

    /**  Class for text events
     */
    class FIFE_API TextEvent : public InputEvent
    {
    public:
        enum TextEventType : uint8_t
        {
            UNKNOWN = 0,
            INPUT   = 1,
            EDIT    = 2
        };

        /** Constructor
         */
        TextEvent() : m_eventType(UNKNOWN) { }

        /** Destructor.
         */
        virtual ~TextEvent() = default;

        TextEventType getType() const
        {
            return m_eventType;
        }
        void setType(TextEventType type)
        {
            m_eventType = type;
        }

        const Text& getText() const
        {
            return m_text;
        }
        void setText(const Text& text)
        {
            m_text = text;
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

        virtual const std::string& getName() const
        {
            static const std::string eventName("TextEvent");
            return eventName;
        }
        virtual std::string getDebugString() const
        {
            return InputEvent::getDebugString();
        }

    private:
        TextEventType m_eventType;
        Text m_text;
    };

} // namespace FIFE

#endif
