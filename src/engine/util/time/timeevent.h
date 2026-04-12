// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_TIMEVENT_H
#define FIFE_TIMEVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
// 3rd party library includes

// FIFE includes
#include "util/base/fife_stdint.h"

namespace FIFE
{

    /** Interface for events to be registered with TimeManager.
     *
     * To register a class with TimeManager firstly derive a class
     * from this and override the updateEvent() function. updateEvent()
     * will be called periodically depending on the value of getPeriod()
     * which can be set using the constructor or setPeriod(). A value
     * of -1 will never be updated, 0 will updated every frame and a value
     * over 0 defines the number of milliseconds between updates.
     *
     * @see TimeManager
     */
    class /*FIFE_API*/ TimeEvent
    {
    public:
        /** Default constructor.
         *
         * @param period The period of the event. See class description.
         */
        explicit TimeEvent(int32_t period = -1);

        /** Destructor.
         *
         */
        virtual ~TimeEvent();

        /** Update function to be overridden by client.
         *
         * @param time Time delta.
         */
        virtual void updateEvent(uint32_t time) = 0;

        /** Called by TimeManager to update the event.
         *
         * @param time Current time. Used To check if its time to update.
         */
        void managerUpdateEvent(uint32_t time);

        /** Set the period of the event.
         *
         * @param period The period of the event. See class description.
         */
        void setPeriod(int32_t period);

        /** Get the period of the event.
         *
         * @return The period of the event. See class description.
         */
        int32_t getPeriod() const;

        /** Get the last time the event was updated.
         *
         * @return Time of last update.
         */
        uint32_t getLastUpdateTime() const;

        /** Set the last time the event was updated.
         *
         * @param ms Time of last update.
         */
        void setLastUpdateTime(uint32_t ms);

    private:
        // The period of the event. See the class description.
        int32_t m_period;

        // The last time the class was updated.
        uint32_t m_last_updated;
    };

} // namespace FIFE

#endif
