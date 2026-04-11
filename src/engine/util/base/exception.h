// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EXCEPTION_H
#define FIFE_EXCEPTION_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <cstddef>
#include <cstring>
#include <stdexcept>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"

namespace FIFE
{

    const std::size_t EXCEPTION_BUFFER_SIZE = 512;

    /**
     * Base class for all FIFE exceptions.
     *
     * Derived exceptions provide more specific error messages.
     */
    class FIFE_API Exception
    {
    public:
        /**
         * Constructor.
         *
         * @param msg The error mesage to be stored (c-string).
         */
        explicit Exception(const char* msg) noexcept
        {
            copy(msg);
        }

        /**
         * Constructor.
         *
         * @param msg The error mesage to be stored (std::string).
         */
        explicit Exception(const std::string& msg) noexcept
        {
            copy(msg.c_str());
        }

        /**
         * Destructor.
         */
        virtual ~Exception() noexcept = default;

        // Copy constructor and assignment operator are defaulted, as the class
        // manages its own buffer and does not require special handling.
        Exception(const Exception&)            = default;
        Exception& operator=(const Exception&) = default;

        virtual const char* what() const noexcept
        {
            return m_buffer;
        }

        virtual const char* getTypeStr() const noexcept
        {
            return "Exception";
        }

        virtual const char* getDescription() const noexcept
        {
            return "Generic FIFE exception";
        }

    private:
        /**
         * Helper function to copy the error message into the internal buffer.
         * Makes sure it does not exceed the buffer size and is null-terminated.
         *
         * @param msg The error message to be copied.
         */
        void copy(const char* msg) noexcept
        {
            if (!msg) {
                m_buffer[0] = '\0';
                return;
            }
            std::strncpy(m_buffer, msg, EXCEPTION_BUFFER_SIZE - 1);
            m_buffer[EXCEPTION_BUFFER_SIZE - 1] = '\0';
        }

    protected:
        char m_buffer[EXCEPTION_BUFFER_SIZE];
    };

    class FIFE_API SDLException : public Exception
    {
    public:
        explicit SDLException(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit SDLException(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "SDLException";
        }
        const char* getDescription() const noexcept override
        {
            return "SDL reported something bad";
        }
    };

    class FIFE_API NotFound : public Exception
    {
    public:
        explicit NotFound(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit NotFound(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "NotFound";
        }
        const char* getDescription() const noexcept override
        {
            return "Something was searched, but not found";
        }
    };

    class FIFE_API NotSet : public Exception
    {
    public:
        explicit NotSet(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit NotSet(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "NotSet";
        }
        const char* getDescription() const noexcept override
        {
            return "Something was not set correctly";
        }
    };

    class FIFE_API IndexOverflow : public Exception
    {
    public:
        explicit IndexOverflow(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit IndexOverflow(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "IndexOverflow";
        }
        const char* getDescription() const noexcept override
        {
            return "Someone tried to access a non-existing element";
        }
    };

    class FIFE_API InvalidFormat : public Exception
    {
    public:
        explicit InvalidFormat(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit InvalidFormat(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "InvalidFormat";
        }
        const char* getDescription() const noexcept override
        {
            return "Found invalid data";
        }
    };

    class FIFE_API CannotOpenFile : public Exception
    {
    public:
        explicit CannotOpenFile(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit CannotOpenFile(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "CannotOpenFile";
        }
        const char* getDescription() const noexcept override
        {
            return "File couldn't be opened";
        }
    };

    class FIFE_API InvalidConversion : public Exception
    {
    public:
        explicit InvalidConversion(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit InvalidConversion(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "InvalidConversion";
        }
        const char* getDescription() const noexcept override
        {
            return "tried an invalid conversion";
        }
    };

    class FIFE_API NotSupported : public Exception
    {
    public:
        explicit NotSupported(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit NotSupported(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "NotSupported";
        }
        const char* getDescription() const noexcept override
        {
            return "This action was not supported";
        }
    };

    class FIFE_API NameClash : public Exception
    {
    public:
        explicit NameClash(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit NameClash(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "NameClash";
        }
        const char* getDescription() const noexcept override
        {
            return "A name or identifier is already in use";
        }
    };

    class FIFE_API Duplicate : public Exception
    {
    public:
        explicit Duplicate(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit Duplicate(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "Duplicate";
        }
        const char* getDescription() const noexcept override
        {
            return "A duplicate item was added, where this is not allowed";
        }
    };

    class FIFE_API ScriptException : public Exception
    {
    public:
        explicit ScriptException(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit ScriptException(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "ScriptException";
        }
        const char* getDescription() const noexcept override
        {
            return "Error related to scripting functionality";
        }
    };

    class FIFE_API EventException : public Exception
    {
    public:
        explicit EventException(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit EventException(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "EventException";
        }
        const char* getDescription() const noexcept override
        {
            return "Error related to event functionality";
        }
    };

    class FIFE_API GuiException : public Exception
    {
    public:
        explicit GuiException(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit GuiException(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "GuiException";
        }
        const char* getDescription() const noexcept override
        {
            return "Error related to gui functionality";
        }
    };

    class FIFE_API InconsistencyDetected : public Exception
    {
    public:
        explicit InconsistencyDetected(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit InconsistencyDetected(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "InconsistencyDetected";
        }
        const char* getDescription() const noexcept override
        {
            return "An inconsistency in FIFE internals was detected. Please report this is a FIFE Bug.";
        }
    };

    class FIFE_API OutOfMemory : public Exception
    {
    public:
        explicit OutOfMemory(const char* msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        explicit OutOfMemory(const std::string& msg) : Exception(msg)
        {
            Logger _log(LM_EXCEPTION);
            FL_ERR(_log, what());
        }
        const char* getTypeStr() const noexcept override
        {
            return "OutOfMemory";
        }
        const char* getDescription() const noexcept override
        {
            return "Error: out of memory. Buy more RAM ;)";
        }
    };

} // namespace FIFE

#endif
