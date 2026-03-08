// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TIMEMANAGER_H
#define FIFE_TIMEMANAGER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fife_stdint.h"
#include "util/base/singleton.h"

namespace FIFE
{

    class TimeEvent;

    /** Time Manager
     *
     * This class is in charge of storing the current time,
     * average frame time, as well as controlling periodic events.
     * Users of this class will have to manually register and
     * unregister events.
     *
     * @see TimeEvent
     */
    class TimeManager : public DynamicSingleton<TimeManager>
    {
    public:
        /** Default constructor.
         */
        TimeManager();

        /** Destructor.
         */
        virtual ~TimeManager();

        /** Called once a frame and updates the timer objects and events.
         */
        void update();

        /** Adds a TimeEvent.
         *
         * The event will be updated regularly, depending on its settings.
         * @param event The TimeEvent object to be added.
         */
        void registerEvent(TimeEvent* event);

        /** Removes a TimeEvent.
         *
         * Removes an event from the list. It will not be deleted.
         * @param event The TimeEvent object to be removed.
         */
        void unregisterEvent(TimeEvent* event);

        /** Get the time.
         *
         * @return The time in milliseconds.
         */
        uint32_t getTime() const;

        /** Get the time since the last frame.
         *
         * @return Time since last frame in milliseconds.
         */
        uint32_t getTimeDelta() const;

        /** Gets average frame time
         *
         * @return Average frame time in milliseconds.
         */
        double getAverageFrameTime() const;

        /** Prints Timer statistics
         */
        void printStatistics() const;

    private:
        /// Current time in milliseconds.
        uint32_t m_current_time;
        /// Time since last frame in milliseconds.
        uint32_t m_time_delta;
        /// Average frame time in milliseconds.
        double m_average_frame_time;

        /// List of active TimeEvents.
        std::vector<TimeEvent*> m_events_list;
    };

} // namespace FIFE

#endif