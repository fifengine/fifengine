// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EVENTCHANNEL_COMMAND_H
#define FIFE_EVENTCHANNEL_COMMAND_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <string>

// 3rd party library includes

// FIFE includes
#include "commandids.h"
#include "eventchannel/base/event.h"

namespace FIFE
{

    /** Class for commands
     * Commands are arbitrary events e.g. send from one subsystem to another
     */
    class /*FIFE_API*/ Command : public Event
    {
    public:
        /** Constructor.
         */
        Command() : m_commandType(CMD_UNKNOWN), m_code(0) { }

        /** Destructor.
         */
        virtual ~Command() = default;

        /** Gets the type of this command
         * @return type of this command
         */
        CommandType getCommandType() const
        {
            return m_commandType;
        }

        /** Sets the type of this command
         */
        void setCommandType(CommandType type)
        {
            m_commandType = type;
        }

        /** Gets the code of this command. Meaning of code depends on the command type
         * @return code of this command
         */
        int32_t getCode() const
        {
            return m_code;
        }

        /** Sets the code of this command. Meaning of code depends on the command type
         */
        void setCode(int32_t code)
        {
            m_code = code;
        }

        /** Marks events as consumed.
         */
        virtual void consume()
        {
            Event::consume();
        }

        /** Checks whether event is consumed.
         */
        virtual bool isConsumed() const
        {
            return Event::isConsumed();
        }

        /** Gets the source of the event.
         */
        virtual IEventSource* getSource() const
        {
            return Event::getSource();
        }

        /** Sets the source of the event.
         */
        virtual void setSource(IEventSource* source)
        {
            Event::setSource(source);
        }

        /** Gets the timestamp of the event.
         */
        virtual int32_t getTimeStamp() const
        {
            return Event::getTimeStamp();
        }

        /** Sets the timestamp of the event.
         */
        virtual void setTimeStamp(int32_t timestamp)
        {
            Event::setTimeStamp(timestamp);
        }

        /** Gets the name of the event.
         */
        virtual const std::string& getName() const
        {
            static const std::string eventName("Command");
            return eventName;
        }

        /** Gets the debugstring of the event.
         */
        virtual std::string getDebugString() const
        {
            return Event::getDebugString();
        }

    private:
        CommandType m_commandType;
        int32_t m_code;
    };
} // namespace FIFE

#endif
