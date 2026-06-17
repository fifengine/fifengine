// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_MOUSEEVENT_H
#define FIFE_EVENTCHANNEL_MOUSEEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/inputevent.h"

namespace FIFE
{

    /**  Class for mouse events
     */
    class FIFE_API MouseEvent : public InputEvent
    {
        public:
            /**
             * Mouse event types.
             */
            enum MouseEventType : int8_t
            {
                UNKNOWN_EVENT     = -1,
                MOVED             = 0,
                PRESSED           = 1,
                RELEASED          = 2,
                WHEEL_MOVED_DOWN  = 3,
                WHEEL_MOVED_UP    = 4,
                WHEEL_MOVED_RIGHT = 5,
                WHEEL_MOVED_LEFT  = 6,
                CLICKED           = 7,
                ENTERED           = 8,
                EXITED            = 9,
                DRAGGED           = 10
            };

            /**
             * Mouse button types.
             */
            enum MouseButtonType : uint8_t
            {
                EMPTY          = 0,
                LEFT           = 1,
                RIGHT          = 2,
                MIDDLE         = 4,
                X1             = 8,
                X2             = 16,
                UNKNOWN_BUTTON = 32
            };

            /** Constructor.
             */
            MouseEvent() : m_eventType(UNKNOWN_EVENT), m_buttonType(UNKNOWN_BUTTON), m_x(-1), m_y(-1)
            {
            }

            /** Destructor.
             */
            ~MouseEvent() override = default;

            /**
             * Gets the button of the mouse event.
             * @return the button of the mouse event.
             */
            MouseButtonType getButton() const
            {
                return m_buttonType;
            }
            void setButton(MouseButtonType type)
            {
                m_buttonType = type;
            }

            /**
             * Gets the type of the event.
             * @return the type of the event.
             */
            MouseEventType getType() const
            {
                return m_eventType;
            }
            void setType(MouseEventType type)
            {
                m_eventType = type;
            }

            /**
             * Gets the x coordinate of the mouse event.
             * @return Logical (point) x coordinate, unaffected by DPI scale.
             *         Multiply by Window::getDPIScaleFactor() for pixel coords.
             */
            int32_t getX() const
            {
                return m_x;
            }
            void setX(int32_t x)
            {
                m_x = x;
            }

            /**
             * Gets the y coordinate of the mouse event.
             * @return Logical (point) y coordinate, unaffected by DPI scale.
             *         Multiply by Window::getDPIScaleFactor() for pixel coords.
             */
            int32_t getY() const
            {
                return m_y;
            }
            void setY(int32_t y)
            {
                m_y = y;
            }

            /** Alias for getX() - returns logical (point) coordinates. */
            int32_t getXInPoints() const
            {
                return getX();
            }
            /** Alias for getY() - returns logical (point) coordinates. */
            int32_t getYInPoints() const
            {
                return getY();
            }

            /** Returns pixel x coordinate, scaled by the given DPI factor. */
            int32_t getXInPixels(float dpiScale) const
            {
                return static_cast<int32_t>(getX() * dpiScale);
            }
            /** Returns pixel y coordinate, scaled by the given DPI factor. */
            int32_t getYInPixels(float dpiScale) const
            {
                return static_cast<int32_t>(getY() * dpiScale);
            }

            std::string const & getName() const override
            {
                static std::string const eventName("MouseEvent");
                return eventName;
            }
            std::string getAttrStr() const override
            {
                std::stringstream ss;
                ss << InputEvent::getAttrStr() << '\n';
                ss << "event = " << mouseEventType2str(m_eventType) << ", ";
                ss << "button = " << mouseButtonType2str(m_buttonType) << ", ";
                ss << "x = " << m_x << ", ";
                ss << "y = " << m_y;
                return ss.str();
            }

            /** Returns string representation of given event type
             */
            static std::string mouseEventType2str(MouseEventType t)
            {
                std::string s("unknown");
                switch (t) {
                case MouseEvent::MOVED:
                    s = "moved";
                    break;
                case MouseEvent::PRESSED:
                    s = "pressed";
                    break;
                case MouseEvent::RELEASED:
                    s = "released";
                    break;
                case MouseEvent::WHEEL_MOVED_DOWN:
                    s = "wheel_moved_down";
                    break;
                case MouseEvent::WHEEL_MOVED_UP:
                    s = "wheel_moved_up";
                    break;
                case MouseEvent::WHEEL_MOVED_RIGHT:
                    s = "wheel_moved_right";
                    break;
                case MouseEvent::WHEEL_MOVED_LEFT:
                    s = "wheel_moved_left";
                    break;
                case MouseEvent::CLICKED:
                    s = "clicked";
                    break;
                case MouseEvent::ENTERED:
                    s = "entered";
                    break;
                case MouseEvent::EXITED:
                    s = "excited";
                    break;
                case MouseEvent::DRAGGED:
                    s = "dragged";
                    break;
                case MouseEvent::UNKNOWN_EVENT:
                    s = "unknown";
                    break;
                default:
                    break;
                }
                return s;
            }

            /** Returns string representation of given button type
             */
            static std::string mouseButtonType2str(MouseButtonType t)
            {
                std::string s("unknown");
                switch (t) {
                case MouseEvent::EMPTY:
                    s = "empty";
                    break;
                case MouseEvent::LEFT:
                    s = "left";
                    break;
                case MouseEvent::RIGHT:
                    s = "right";
                    break;
                case MouseEvent::MIDDLE:
                    s = "middle";
                    break;
                case MouseEvent::X1:
                    s = "x1";
                    break;
                case MouseEvent::X2:
                    s = "x2";
                    break;
                case MouseEvent::UNKNOWN_BUTTON:
                    s = "unknown button";
                    break;
                default:
                    break;
                }
                return s;
            }

        private:
            MouseEventType m_eventType;
            MouseButtonType m_buttonType;
            int32_t m_x;
            int32_t m_y;
    };

} // namespace FIFE

#endif
