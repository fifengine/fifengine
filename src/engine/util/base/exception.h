// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EXCEPTION_H
#define FIFE_EXCEPTION_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <stdexcept>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/log/logger.h"

namespace FIFE
{

    /**
     * Base class for all FIFE exceptions.
     *
     * Derived exceptions provide more specific error messages.
     *
     * Do not export this class with FIFE_API to avoid DLL export warnings.
     */
    class /*FIFE_API*/ Exception : public std::runtime_error
    {
    public:
        /**
         * Constructor.
         *
         * @param msg The error mesage to be stored.
         */
        explicit Exception(const std::string& msg);

        /**
         * Destructor.
         */
        virtual ~Exception() noexcept;

        Exception(const Exception&)            = default;
        Exception& operator=(const Exception&) = default;

        /**
         * Returns the error message.
         *
         * @return The error message.
         */
        virtual const char* what() const noexcept;

        virtual const std::string& getTypeStr() const
        {
            static const std::string s = "Exception";
            return s;
        }
        virtual const std::string& getDescription() const
        {
            static const std::string s = "Generic FIFE exception";
            return s;
        }
        // little helper to change m_what
        void update();

    private:
        std::string m_what;
    };

// NOLINTNEXTLINE(bugprone-macro-parentheses,cppcoreguidelines-avoid-do-while)
#define FIFE_EXCEPTION_DECL(_name, _description)                  \
    class FIFE_API _name : public Exception                       \
    {                                                             \
    public:                                                       \
        explicit _name(const std::string& msg) : Exception((msg)) \
        {                                                         \
            Logger _log(LM_EXCEPTION);                            \
            update();                                             \
            FL_ERR(_log, what());                                 \
        }                                                         \
        const std::string& getTypeStr() const override            \
        {                                                         \
            static const std::string s = #_name;                  \
            return s;                                             \
        }                                                         \
        const std::string& getDescription() const override        \
        {                                                         \
            static const std::string s = (_description);          \
            return s;                                             \
        }                                                         \
    }

    FIFE_EXCEPTION_DECL(SDLException, "SDL reported something bad");
    FIFE_EXCEPTION_DECL(NotFound, "Something was searched, but not found");
    FIFE_EXCEPTION_DECL(NotSet, "Something was not set correctly");
    FIFE_EXCEPTION_DECL(IndexOverflow, "Someone tried to access a non-existing element");
    FIFE_EXCEPTION_DECL(InvalidFormat, "Found invalid data");
    FIFE_EXCEPTION_DECL(CannotOpenFile, "File couldn't be opened");
    FIFE_EXCEPTION_DECL(InvalidConversion, "Tried an invalid conversion");
    FIFE_EXCEPTION_DECL(NotSupported, "This action was not supported");
    FIFE_EXCEPTION_DECL(NameClash, "A name or identifier is already in use");
    FIFE_EXCEPTION_DECL(Duplicate, "A duplicate item was added, where this is not allowed");
    FIFE_EXCEPTION_DECL(ScriptException, "Error related to scripting functionality");
    FIFE_EXCEPTION_DECL(EventException, "Error related to event functionality");
    FIFE_EXCEPTION_DECL(GuiException, "Error related to gui functionality");
    FIFE_EXCEPTION_DECL(
        InconsistencyDetected, "An inconsistency in FIFE internals was detected. Please report this is a FIFE Bug.");

    FIFE_EXCEPTION_DECL(OutOfMemory, "Error: out of memory. Buy more RAM ;)");

} // namespace FIFE

#endif
