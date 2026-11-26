// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_TEST_ENV_H
#define FIFE_GLCORE_TEST_ENV_H

#include <cstdlib>
#include <string>

#include <catch2/catch_test_macros.hpp>

namespace glcore_test
{
    /** In a configured GL environment (FIFE_GL_REQUIRE_CONTEXT=1) a missing
     * context is a failure; otherwise the case is skipped so builds without
     * any GL implementation still pass.
     */
    inline void skipOrFail(char const * reason)
    {
#ifdef _MSC_VER
        char* required = nullptr;
        size_t length  = 0;
        std::string flag;
        if (_dupenv_s(&required, &length, "FIFE_GL_REQUIRE_CONTEXT") == 0) {
            flag = required != nullptr ? required : "";
        }
        std::free(required);
#else
        char const * required  = std::getenv("FIFE_GL_REQUIRE_CONTEXT");
        std::string const flag = required != nullptr ? required : "";
#endif
        if (flag == "1" || flag == "true" || flag == "TRUE") {
            FAIL(reason);
        }
        SKIP(reason);
    }

    /** Core-profile-only cases are compiled in the ES2 build too; there they
     * must skip instead of failing even in a configured GL environment.
     */
    inline void skipIfOtherProfile(char const * reason)
    {
#ifdef HAVE_OPENGL_ES2
        SKIP(reason);
#else
        skipOrFail(reason);
#endif
    }

    /** ES2-only cases are compiled in the core build too and must skip there. */
    inline void skipIfES2Only(char const * reason)
    {
#ifdef HAVE_OPENGL_CORE
        SKIP(reason);
#else
        skipOrFail(reason);
#endif
    }
} // namespace glcore_test

#endif
