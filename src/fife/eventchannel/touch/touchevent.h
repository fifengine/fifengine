// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_TOUCH_TOUCHEVENT_H
#define FIFE_EVENTCHANNEL_TOUCH_TOUCHEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>
#include <sstream>
#include <string>

// 3rd party library includes

// FIFE includes
#include "eventchannel/base/inputevent.h"

namespace FIFE
{

    /** Raw touch and pinch events.
     *
     * Mouse listeners keep receiving synthesized mouse events for compatibility.
     * A touch listener receives the original finger/pinch data as well, which is
     * required for gestures a mouse event cannot represent.
     */
    class FIFE_API TouchEvent : public InputEvent
    {
        public:
            enum TouchEventType : int8_t
            {
                UNKNOWN      = -1,
                DOWN         = 0,
                UP           = 1,
                MOTION       = 2,
                CANCELED     = 3,
                PINCH_BEGIN  = 4,
                PINCH_UPDATE = 5,
                PINCH_END    = 6
            };

            TouchEvent() :
                m_eventType(UNKNOWN),
                m_touchId(0),
                m_fingerId(0),
                m_x(-1),
                m_y(-1),
                m_normalizedX(0.0F),
                m_normalizedY(0.0F),
                m_deltaX(0.0F),
                m_deltaY(0.0F),
                m_pressure(0.0F),
                m_scale(1.0F),
                m_fingerCount(0),
                m_trackpad(false)
            {
            }

            ~TouchEvent() override = default;

            TouchEventType getType() const
            {
                return m_eventType;
            }
            void setType(TouchEventType type)
            {
                m_eventType = type;
            }

            /** SDL touch device id. Zero for pinch events. */
            uint64_t getTouchId() const
            {
                return m_touchId;
            }
            void setTouchId(uint64_t touchId)
            {
                m_touchId = touchId;
            }

            /** SDL finger id, unique within the touch device. */
            uint64_t getFingerId() const
            {
                return m_fingerId;
            }
            void setFingerId(uint64_t fingerId)
            {
                m_fingerId = fingerId;
            }

            /** Logical (point) coordinates, same unit as MouseEvent. */
            int32_t getX() const
            {
                return m_x;
            }
            void setX(int32_t x)
            {
                m_x = x;
            }

            /** Logical (point) coordinates, same unit as MouseEvent. */
            int32_t getY() const
            {
                return m_y;
            }
            void setY(int32_t y)
            {
                m_y = y;
            }

            /** Raw SDL normalized position in the range 0..1. */
            float getNormalizedX() const
            {
                return m_normalizedX;
            }
            void setNormalizedX(float normalizedX)
            {
                m_normalizedX = normalizedX;
            }

            /** Raw SDL normalized position in the range 0..1. */
            float getNormalizedY() const
            {
                return m_normalizedY;
            }
            void setNormalizedY(float normalizedY)
            {
                m_normalizedY = normalizedY;
            }

            /** Raw SDL normalized motion delta in the range -1..1. */
            float getDeltaX() const
            {
                return m_deltaX;
            }
            void setDeltaX(float deltaX)
            {
                m_deltaX = deltaX;
            }

            /** Raw SDL normalized motion delta in the range -1..1. */
            float getDeltaY() const
            {
                return m_deltaY;
            }
            void setDeltaY(float deltaY)
            {
                m_deltaY = deltaY;
            }

            float getPressure() const
            {
                return m_pressure;
            }
            void setPressure(float pressure)
            {
                m_pressure = pressure;
            }

            /** Accumulated pinch scale for PINCH_* events.
             *
             * Values below 1 mean zoom out, values above 1 mean zoom in. The
             * default is 1 for non-pinch events.
             */
            float getScale() const
            {
                return m_scale;
            }
            void setScale(float scale)
            {
                m_scale = scale;
            }

            /** Number of active fingers on the same touch device. */
            int32_t getFingerCount() const
            {
                return m_fingerCount;
            }
            void setFingerCount(int32_t fingerCount)
            {
                m_fingerCount = fingerCount;
            }

            /** True when the touch device is an indirect device (trackpad). */
            bool isTrackpad() const
            {
                return m_trackpad;
            }
            void setTrackpad(bool trackpad)
            {
                m_trackpad = trackpad;
            }

            /** True for PINCH_BEGIN/PINCH_UPDATE/PINCH_END. */
            bool isPinch() const
            {
                return (m_eventType == PINCH_BEGIN) || (m_eventType == PINCH_UPDATE) || (m_eventType == PINCH_END);
            }

            std::string const & getName() const override
            {
                static std::string const eventName("TouchEvent");
                return eventName;
            }

            std::string getAttrStr() const override
            {
                std::stringstream ss;
                ss << InputEvent::getAttrStr() << '\n';
                ss << "event = " << touchEventType2str(m_eventType) << ", ";
                ss << "touchId = " << m_touchId << ", ";
                ss << "fingerId = " << m_fingerId << ", ";
                ss << "x = " << m_x << ", ";
                ss << "y = " << m_y << ", ";
                ss << "normalizedX = " << m_normalizedX << ", ";
                ss << "normalizedY = " << m_normalizedY << ", ";
                ss << "deltaX = " << m_deltaX << ", ";
                ss << "deltaY = " << m_deltaY << ", ";
                ss << "pressure = " << m_pressure << ", ";
                ss << "scale = " << m_scale << ", ";
                ss << "fingerCount = " << m_fingerCount << ", ";
                ss << "trackpad = " << m_trackpad;
                return ss.str();
            }

            static std::string touchEventType2str(TouchEventType type)
            {
                switch (type) {
                case DOWN:
                    return "down";
                case UP:
                    return "up";
                case MOTION:
                    return "motion";
                case CANCELED:
                    return "canceled";
                case PINCH_BEGIN:
                    return "pinch_begin";
                case PINCH_UPDATE:
                    return "pinch_update";
                case PINCH_END:
                    return "pinch_end";
                case UNKNOWN:
                default:
                    return "unknown";
                }
            }

        private:
            TouchEventType m_eventType;
            uint64_t m_touchId;
            uint64_t m_fingerId;
            int32_t m_x;
            int32_t m_y;
            float m_normalizedX;
            float m_normalizedY;
            float m_deltaX;
            float m_deltaY;
            float m_pressure;
            float m_scale;
            int32_t m_fingerCount;
            bool m_trackpad;
    };

} // namespace FIFE

#endif
