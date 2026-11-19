// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "eventchannel/touch/touchtranslation.h"

// Standard C++ library includes
#include <algorithm>
#include <cmath>

namespace FIFE
{
    namespace TouchTranslation
    {

        PointI normalizedToWindowPoints(float normalizedX, float normalizedY, int32_t width, int32_t height)
        {
            PointI point;
            if (width <= 0 || height <= 0) {
                return point;
            }

            float const clampedX = std::clamp(normalizedX, 0.0F, 1.0F);
            float const clampedY = std::clamp(normalizedY, 0.0F, 1.0F);

            point.x = static_cast<int32_t>(std::lround(static_cast<double>(clampedX) * width));
            point.y = static_cast<int32_t>(std::lround(static_cast<double>(clampedY) * height));
            return point;
        }

        ScrollStep classifyScroll(float deltaX, float deltaY, float deadZone)
        {
            ScrollStep step;
            step.deltaX = deltaX;
            step.deltaY = deltaY;

            float const absX = std::abs(deltaX);
            float const absY = std::abs(deltaY);

            if ((absX == 0.0F) && (absY == 0.0F)) {
                return step;
            }

            float const threshold = std::max(deadZone, 0.0F);

            // Vertical wins ties: a diagonal jitter must not turn an intentional
            // vertical pan into a horizontal wheel callback. Vertical pans are
            // never suppressed, so slow scrolling still works.
            if ((absY > 0.0F) && (absY >= absX)) {
                step.axis      = ScrollAxis::VERTICAL;
                step.direction = (deltaY > 0.0F) ? 1 : -1;
                return step;
            }

            if ((absX >= threshold) && (absX > absY)) {
                step.axis      = ScrollAxis::HORIZONTAL;
                step.direction = (deltaX > 0.0F) ? 1 : -1;
            }

            return step;
        }

        MouseEvent::MouseEventType wheelTypeFor(ScrollStep const & step)
        {
            switch (step.axis) {
            case ScrollAxis::HORIZONTAL:
                return (step.direction > 0) ? MouseEvent::WHEEL_MOVED_RIGHT : MouseEvent::WHEEL_MOVED_LEFT;
            case ScrollAxis::VERTICAL:
                return (step.direction > 0) ? MouseEvent::WHEEL_MOVED_DOWN : MouseEvent::WHEEL_MOVED_UP;
            case ScrollAxis::NONE:
            default:
                return MouseEvent::UNKNOWN_EVENT;
            }
        }

        bool isMouseEventType(uint32_t sdlEventType)
        {
            switch (sdlEventType) {
            case SDL_EVENT_MOUSE_MOTION:
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_WHEEL:
                return true;
            default:
                return false;
            }
        }

        bool isTouchSynthesizedMouse(SDL_Event const & event)
        {
            switch (event.type) {
            case SDL_EVENT_MOUSE_MOTION:
                return event.motion.which == SDL_TOUCH_MOUSEID;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                return event.button.which == SDL_TOUCH_MOUSEID;
            case SDL_EVENT_MOUSE_WHEEL:
                return event.wheel.which == SDL_TOUCH_MOUSEID;
            default:
                return false;
            }
        }

    } // namespace TouchTranslation
} // namespace FIFE
