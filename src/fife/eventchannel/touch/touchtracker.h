// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_TOUCH_TOUCHTRACKER_H
#define FIFE_EVENTCHANNEL_TOUCH_TOUCHTRACKER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstddef>
#include <cstdint>
#include <map>
#include <utility>

// 3rd party library includes
#include <SDL3/SDL.h>

namespace FIFE
{

    /** State of a single active finger. */
    struct TouchFinger
    {
            SDL_TouchID touchId   = 0;
            SDL_FingerID fingerId = 0;
            float x               = 0.0F; // normalized 0..1
            float y               = 0.0F; // normalized 0..1
            float pressure        = 0.0F; // normalized 0..1
    };

    /** Tracks active fingers.
     *
     * Fingers are keyed by the (touchID, fingerID) pair. SDL only guarantees
     * that finger IDs are unique within a single touch device, so keying by
     * fingerID alone would mix fingers from a touch screen and a trackpad.
     */
    class FIFE_API TouchTracker
    {
        public:
            /** Adds a finger. Returns true when it was not tracked before.
             *  A repeated begin() for the same identity updates the position.
             */
            bool begin(SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float pressure);

            /** Updates a finger. Returns false when the identity is unknown
             *  (for example after a missed FINGER_UP).
             */
            bool update(SDL_TouchID touchId, SDL_FingerID fingerId, float x, float y, float pressure);

            /** Removes a finger. Returns true when the identity was tracked. */
            bool end(SDL_TouchID touchId, SDL_FingerID fingerId);

            /** Removes all fingers and forgotten device types. */
            void clear();

            bool contains(SDL_TouchID touchId, SDL_FingerID fingerId) const;

            /** Number of active fingers across all touch devices. */
            std::size_t fingerCount() const;

            /** Number of active fingers on one touch device. */
            std::size_t fingerCount(SDL_TouchID touchId) const;

            /** Normalized midpoint of all active fingers.
             *  @return false when no finger is active.
             */
            bool midpoint(float& x, float& y) const;

            void setDeviceType(SDL_TouchID touchId, SDL_TouchDeviceType type);

            /** A trackpad is an indirect touch device. Unknown devices are
             *  treated as direct touch screens.
             */
            bool isTrackpad(SDL_TouchID touchId) const;

        private:
            using Key = std::pair<SDL_TouchID, SDL_FingerID>;

            std::map<Key, TouchFinger> m_fingers;
            std::map<SDL_TouchID, SDL_TouchDeviceType> m_deviceTypes;
    };

    /** Accumulates relative pinch scale for the duration of one gesture.
     *
     * SDL_PinchFingerEvent.scale is relative to the previous update, so the
     * zoom factor has to be multiplied rather than overwritten.
     */
    class FIFE_API PinchTracker
    {
        public:
            void begin();
            /** Multiplies the accumulated scale. Returns false when no gesture
             *  is active.
             */
            bool update(float scale);
            void end();

            bool isActive() const
            {
                return m_active;
            }

            float scale() const
            {
                return m_scale;
            }

        private:
            bool m_active = false;
            float m_scale = 1.0F;
    };

} // namespace FIFE

#endif
