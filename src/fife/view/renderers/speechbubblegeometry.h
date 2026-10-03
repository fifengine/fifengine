// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_SPEECHBUBBLEGEOMETRY_H
#define FIFE_SPEECHBUBBLEGEOMETRY_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <array>
#include <cstdlib>

// 3rd party library includes

// FIFE includes
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "view/renderers/speechstyle.h"

namespace FIFE
{
    /**
     * Pure geometry helpers for speech bubbles.
     *  Kept free of renderer state so they can be shared and unit tested.
     */
    namespace detail
    {
        /**
         * Expands the text rect by bubblePadding on every side.
         */
        inline Rect computeBubbleBounds(Rect const & textBounds, SpeechStyle const & style)
        {
            int32_t const pad = std::max(0, style.bubblePadding);
            return Rect(textBounds.x - pad, textBounds.y - pad, textBounds.w + 2 * pad, textBounds.h + 2 * pad);
        }

        /**
         * Resolves TailDirection::AUTO against the instance anchor.
         *  Picks the axis the anchor is furthest away on; ties favour DOWN.
         */
        inline TailDirection inferTailDirection(Point const & instanceAnchor, Rect const & bubbleRect)
        {
            int32_t const cx  = bubbleRect.x + bubbleRect.w / 2;
            int32_t const cy  = bubbleRect.y + bubbleRect.h / 2;
            int32_t const dx  = instanceAnchor.x - cx;
            int32_t const dy  = instanceAnchor.y - cy;
            int32_t const adx = dx < 0 ? -dx : dx;
            int32_t const ady = dy < 0 ? -dy : dy;

            if (adx > ady) {
                return dx > 0 ? TailDirection::DOWN : TailDirection::UP;
            }
            return dy >= 0 ? TailDirection::DOWN : TailDirection::UP;
        }

        /**
         * Resolves a requested tail direction, inferring it for AUTO.
         */
        inline TailDirection resolveTailDirection(
            TailDirection requested, Point const & instanceAnchor, Rect const & bubbleRect)
        {
            return requested == TailDirection::AUTO ? inferTailDirection(instanceAnchor, bubbleRect) : requested;
        }

        /**
         * Computes the tail triangle: index 0 is the tip, 1 and 2 the base on the bubble edge.
         */
        inline std::array<Point, 3> computeTailVertices(
            Rect const & bubbleRect, Point const & tip, TailDirection dir, int32_t tailSize)
        {
            int32_t const halfBase = std::max(0, tailSize) / 2;

            switch (dir) {
            case TailDirection::UP:
                return {tip, Point(tip.x - halfBase, bubbleRect.y), Point(tip.x + halfBase, bubbleRect.y)};
            case TailDirection::LEFT:
                return {tip, Point(bubbleRect.x, tip.y - halfBase), Point(bubbleRect.x, tip.y + halfBase)};
            case TailDirection::RIGHT:
                return {tip, Point(bubbleRect.right(), tip.y - halfBase), Point(bubbleRect.right(), tip.y + halfBase)};
            case TailDirection::DOWN:
            case TailDirection::AUTO:
            default:
                return {
                    tip, Point(tip.x - halfBase, bubbleRect.bottom()), Point(tip.x + halfBase, bubbleRect.bottom())};
            }
        }

        /**
         * Clamps the corner radius so opposing corners cannot overlap.
         */
        inline int32_t clampCornerRadius(Rect const & bubbleRect, int32_t cornerRadius)
        {
            int32_t const halfW = bubbleRect.w / 2;
            int32_t const halfH = bubbleRect.h / 2;
            return std::max(0, std::min(cornerRadius, std::min(halfW, halfH)));
        }
    } // namespace detail
} // namespace FIFE

#endif
