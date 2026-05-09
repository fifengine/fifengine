// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

/***************************************************************************
 * Note! FIFE event channel borrows heavily from ideas of Guichan library  *
 * version 0.6                                                             *
 ***************************************************************************/

#ifndef FIFE_EVENTCHANNEL_EVENT_H
#define FIFE_EVENTCHANNEL_EVENT_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
//
#include <sstream>
#include <string>

// 3rd party library includes
//
#include <SDL3/SDL.h>

// FIFE includes
//
#include "eventchannel/source/ieventsource.h"
#include "util/time/sdltimecompat.h"
#include "util/time/timemanager.h"

namespace FIFE
{
    /**  Base class for all events
     */
    class FIFE_API Event
    {
        public:
            /** Constructor.
             */
            Event() : m_isConsumed(false), m_eventSource(nullptr), m_timestamp64(TimeManager::instance()->getTicks64())
            {
            }

            /** Destructor.
             */
            virtual ~Event() = default;

            /** Marks the event as consumed.
             */
            virtual void consume()
            {
                m_isConsumed = true;
            }

            /** Checks if the event is consumed.
             * @return true if the event is consumed, false otherwise.
             */
            virtual bool isConsumed() const
            {
                return m_isConsumed;
            }

            /** Gets the source of the event.
             */
            virtual IEventSource* getSource() const
            {
                return m_eventSource;
            }

            /** Sets the source of the event.
             */
            virtual void setSource(IEventSource* source)
            {
                m_eventSource = source;
            }

            /** Gets the timestamp of the event
             */
            [[deprecated("Use getTimeStamp64() instead.")]] virtual int32_t getTimeStamp() const
            {
                return SDLTimeCompat::toInt32Ticks(m_timestamp64);
            }

            /** Gets the timestamp of the event as 64-bit SDL ticks.
             */
            virtual Uint64 getTimeStamp64() const
            {
                return m_timestamp64;
            }

            /** Sets the timestamp of the event
             */
            [[deprecated("Use setTimeStamp64() instead.")]] virtual void setTimeStamp(int32_t timestamp)
            {
                m_timestamp64 = SDLTimeCompat::fromLegacy32Ticks(timestamp);
            }

            /** Sets the timestamp of the event as 64-bit SDL ticks.
             */
            virtual void setTimeStamp64(Uint64 timestamp)
            {
                m_timestamp64 = timestamp;
            }

            /** Gets the name of the event
             */
            virtual std::string const & getName() const
            {
                static std::string const eventName("Event");
                return eventName;
            }

            /** Gets attribute string of the event
             */
            virtual std::string getAttrStr() const
            {
                std::stringstream ss;
                ss << "consumed = " << m_isConsumed << ", ";
                ss << "src = " << m_eventSource << ", ";
                ss << "timestamp = " << SDLTimeCompat::toInt32Ticks(m_timestamp64);
                ss << ", timestamp64 = " << getTimeStamp64();
                return ss.str();
            }

            /** Gets the debugstring of the event
             */
            virtual std::string getDebugString() const
            {
                std::stringstream ss;
                ss << getName() << '\n';
                ss << getAttrStr() << '\n';
                return ss.str();
            }

        private:
            //! Indicates if the event is consumed.
            bool m_isConsumed;
            //! The source of the event.
            IEventSource* m_eventSource;
            //! Timestamp of the event in SDL ticks.
            Uint64 m_timestamp64;
    };

} // namespace FIFE

#endif
