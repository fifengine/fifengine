// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_JOYSTICKEVENT_H
#define FIFE_EVENTCHANNEL_JOYSTICKEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/inputevent.h"
#include "eventchannel/source/ieventsource.h"

#include "joystick.h"

namespace FIFE
{

    /**  Class for Joystick events
     */
    class FIFE_API JoystickEvent : public InputEvent
    {
    public:
        enum JoystickEventType : uint8_t
        {
            UNKNOWN_EVENT = 0,
            AXIS_MOTION,
            HAT_MOTION,
            BUTTON_PRESSED,
            BUTTON_RELEASED,
            DEVICE_ADDED,
            DEVICE_REMOVED
        };

        /** Constructor
         */
        JoystickEvent() :
            m_eventType(UNKNOWN_EVENT),
            m_instanceId(-1),
            m_axis(-1),
            m_axisValue(0),
            m_hat(-1),
            m_hatValue(-1),
            m_button(-1),
            m_controller(false)
        {
        }

        /** Destructor.
         */
        virtual ~JoystickEvent() = default;

        /** Return the event type.
         */
        JoystickEventType getType() const
        {
            return m_eventType;
        }

        /** Sets the event type.
         */
        void setType(JoystickEventType type)
        {
            m_eventType = type;
        }

        /** Return the instance id of the joystick.
         */
        int32_t getInstanceId() const
        {
            return m_instanceId;
        }

        /** Sets the instance id of the joystick.
         */
        void setInstanceId(int32_t id)
        {
            m_instanceId = id;
        }

        /** Return the index number of axis for joystick, or the Joystick::ContollerAxis for controller.
         */
        int16_t getAxis() const
        {
            return m_axis;
        }

        /** Sets the axis index number or the Joystick::ControllerAxis.
         */
        void setAxis(int16_t axis)
        {
            m_axis = axis;
        }

        /** Return the axis value, range is -1.0 to 1.0.
         */
        float getAxisValue() const
        {
            return m_axisValue;
        }

        /** Sets the axis value.
         */
        void setAxisValue(float value)
        {
            m_axisValue = value;
        }

        /** Return the hat index.
         */
        int16_t getHat() const
        {
            return m_hat;
        }

        /** Sets the hat index.
         */
        void setHat(int16_t hat)
        {
            m_hat = hat;
        }

        /** Return the hat value, see Joystick::Hat.
         */
        int16_t getHatValue() const
        {
            return m_hatValue;
        }

        /** Sets the hat value.
         */
        void setHatValue(int16_t value)
        {
            m_hatValue = value;
        }

        /** Return the button index for joystick, or Joystick::ControllerButton for controller.
         */
        int16_t getButton() const
        {
            return m_button;
        }

        /** Sets the button index or Joystick::ControllerButton.
         */
        void setButton(int16_t button)
        {
            m_button = button;
        }

        /** Return true if it is a controller, false if only a joystick.
         */
        bool isController() const
        {
            return m_controller;
        }

        /** Sets to true if the event is for a controller, otherwise false.
         */
        void setController(bool value)
        {
            m_controller = value;
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
            static const std::string eventName("JostickEvent");
            return eventName;
        }
        virtual std::string getDebugString() const
        {
            return InputEvent::getDebugString();
        }

    private:
        //! Event type
        JoystickEventType m_eventType;
        //! Joystick instance id
        int32_t m_instanceId;
        //! Axis index
        int16_t m_axis;
        //! Axis value
        float m_axisValue;
        //! Hat index
        int16_t m_hat;
        //! Hat value
        int16_t m_hatValue;
        //! Button index
        int16_t m_button;
        //! Is gamecontroller
        bool m_controller;
    };

} // namespace FIFE

#endif
