// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

/***************************************************************************
 * Note! FIFE event channel borrows heavily from ideas of Guichan library  *
 * version 0.6                                                             *
 ***************************************************************************/

#ifndef FIFE_EVENTCHANNEL_EVENT_H
#define FIFE_EVENTCHANNEL_EVENT_H

// Standard C++ library includes
//
#include <sstream>
#include <string>

// 3rd party library includes
//
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
//
#include "eventchannel/source/ieventsource.h"

namespace FIFE
{
    /**  Base class for all events
     */
    class Event
    {
    public:
        /** Constructor.
         */
        Event() : m_isConsumed(false), m_eventSource(nullptr), m_timestamp(SDL_GetTicks()) { }

        /** Destructor.
         */
        virtual ~Event() { }

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
        virtual int32_t getTimeStamp() const
        {
            return m_timestamp;
        }

        /** Sets the timestamp of the event
         */
        virtual void setTimeStamp(int32_t timestamp)
        {
            m_timestamp = timestamp;
        }

        /** Gets the name of the event
         */
        virtual const std::string& getName() const
        {
            const static std::string eventName("Event");
            return eventName;
        }

        /** Gets attribute string of the event
         */
        virtual std::string getAttrStr() const
        {
            std::stringstream ss;
            ss << "consumed = " << m_isConsumed << ", ";
            ss << "src = " << m_eventSource << ", ";
            ss << "timestamp = " << m_timestamp;
            return ss.str();
        }

        /** Gets the debugstring of the event
         */
        virtual std::string getDebugString() const
        {
            std::stringstream ss;
            ss << getName() << std::endl;
            ss << getAttrStr() << std::endl;
            return ss.str();
        }

    private:
        //! Indicates if the event is consumed.
        bool m_isConsumed;
        //! The source of the event.
        IEventSource* m_eventSource;
        //! Timestamp of the event.
        int32_t m_timestamp;
    };

} // namespace FIFE

#endif
