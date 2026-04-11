// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_INPUTEVENT_H
#define FIFE_EVENTCHANNEL_INPUTEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "event.h"

namespace FIFE
{

    /**  Base class for input events (like mouse and keyboard)
     */
    class FIFE_API InputEvent : public Event
    {
    public:
        /** Constructor.
         */
        InputEvent() :
            m_consumedByWidgets(false),
            m_isShiftPressed(false),
            m_isControlPressed(false),
            m_isAltPressed(false),
            m_isMetaPressed(false)
        {
        }

        /** Destructor.
         */
        ~InputEvent() = default;

        /** Checks whether alt is pressed.
         */
        virtual bool isAltPressed() const
        {
            return m_isAltPressed;
        }

        /** Sets alt to pressed.
         */
        virtual void setAltPressed(bool pressed)
        {
            m_isAltPressed = pressed;
        }

        /** Checks whether control is pressed.
         */
        virtual bool isControlPressed() const
        {
            return m_isControlPressed;
        }

        /** Sets control to pressed.
         */
        virtual void setControlPressed(bool pressed)
        {
            m_isControlPressed = pressed;
        }

        /** Checks whether meta is pressed.
         */
        virtual bool isMetaPressed() const
        {
            return m_isMetaPressed;
        }

        /** Sets meta to pressed.
         */
        virtual void setMetaPressed(bool pressed)
        {
            m_isMetaPressed = pressed;
        }

        /** Checks whether shift is pressed.
         */
        virtual bool isShiftPressed() const
        {
            return m_isShiftPressed;
        }

        /** Sets shift to pressed.
         */
        virtual void setShiftPressed(bool pressed)
        {
            m_isShiftPressed = pressed;
        }

        /** Marks events as consumed by widget library.
         */
        virtual void consumedByWidgets()
        {
            m_consumedByWidgets = true;
        }

        /** Checks whether event is consumed by widget library.
         */
        virtual bool isConsumedByWidgets() const
        {
            return m_consumedByWidgets;
        }

        /** Marks events as consumed.
         */
        virtual void consume()
        {
            Event::consume();
        }

        /** Checks whether event is consumed.
         */
        virtual bool isConsumed() const
        {
            return Event::isConsumed();
        }

        /** Gets the source of the event.
         */
        virtual IEventSource* getSource() const
        {
            return Event::getSource();
        }

        /** Sets the source of the event.
         */
        virtual void setSource(IEventSource* source)
        {
            Event::setSource(source);
        }

        /** Gets the timestamp of the event.
         */
        virtual int32_t getTimeStamp() const
        {
            return Event::getTimeStamp();
        }

        /** Sets the timestamp of the event.
         */
        virtual void setTimeStamp(int32_t timestamp)
        {
            Event::setTimeStamp(timestamp);
        }

        /** Gets the name of the event.
         */
        virtual const std::string& getName() const
        {
            static const std::string eventName("InputEvent");
            return eventName;
        }

        /** Gets the debugstring of the event.
         */
        virtual std::string getDebugString() const
        {
            return Event::getDebugString();
        }

        /** Gets attribute string of the event.
         */
        virtual std::string getAttrStr() const
        {
            std::stringstream ss;
            ss << Event::getAttrStr() << '\n';
            ss << "shift = " << m_isShiftPressed << ", ";
            ss << "ctrl = " << m_isControlPressed << ", ";
            ss << "alt = " << m_isAltPressed << ", ";
            ss << "meta = " << m_isMetaPressed;
            return ss.str();
        }

    private:
        bool m_consumedByWidgets;
        bool m_isShiftPressed;
        bool m_isControlPressed;
        bool m_isAltPressed;
        bool m_isMetaPressed;
    };
} // namespace FIFE

#endif
