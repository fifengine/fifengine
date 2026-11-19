// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_TOUCH_TOUCHTRANSLATION_H
#define FIFE_EVENTCHANNEL_TOUCH_TOUCHTRANSLATION_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstdint>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "eventchannel/mouse/mouseevent.h"

namespace FIFE
{
    namespace TouchTranslation
    {

        /** Default dead zone for horizontal pans, in normalized window units.
         *  A horizontal pan below this threshold is ignored so small diagonal
         *  jitter cannot produce horizontal wheel callbacks. Vertical pans are
         *  never suppressed.
         */
        inline constexpr float DEFAULT_SCROLL_DEAD_ZONE = 0.01F;

        struct PointI
        {
                int32_t x = 0;
                int32_t y = 0;
        };

        /** Converts SDL normalized touch coordinates (0..1) to window points.
         *
         * SDL explicitly allows values outside 0..1 for touches that leave the
         * window, so the input is clamped first. Truncating a normalized float
         * to int (the old Fifechan bug) would always produce 0 or 1.
         */
        PointI normalizedToWindowPoints(float normalizedX, float normalizedY, int32_t width, int32_t height);

        enum class ScrollAxis : uint8_t
        {
            NONE = 0,
            HORIZONTAL,
            VERTICAL
        };

        struct ScrollStep
        {
                ScrollAxis axis   = ScrollAxis::NONE;
                float deltaX      = 0.0F; // raw normalized delta
                float deltaY      = 0.0F; // raw normalized delta
                int32_t direction = 0;    // +1 right/down, -1 left/up, 0 none
        };

        /** Picks the dominant scroll axis and applies the dead zone.
         *
         * A vertical pan always wins ties. Horizontal pans only win when they
         * clearly exceed the vertical component and cross the dead zone.
         */
        ScrollStep classifyScroll(float deltaX, float deltaY, float deadZone = DEFAULT_SCROLL_DEAD_ZONE);

        /** Maps a scroll step to the legacy coarse MouseEvent type. */
        MouseEvent::MouseEventType wheelTypeFor(ScrollStep const & step);

        /** True for the four SDL mouse event types understood by
         *  EventManager::fillMouseEvent().
         */
        bool isMouseEventType(uint32_t sdlEventType);

        /** True when a mouse event was synthesized by SDL from touch input.
         *
         * Under policy B this should normally never be true while FIFE owns
         * touch translation; it is used as a defensive duplicate-event guard.
         */
        bool isTouchSynthesizedMouse(SDL_Event const & event);

    } // namespace TouchTranslation
} // namespace FIFE

#endif
