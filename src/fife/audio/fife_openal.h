// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FIFE_OPENAL_H
#define FIFE_FIFE_OPENAL_H

#define AL_ALEXT_PROTOTYPES

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

// 3rd party library includes

// FIFE includes

// clang-format off
#ifdef LOG_ENABLED

    #include <format>

    #define CHECK_OPENAL_LOG(logger, level, msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
        if (AL_NO_ERROR != alGetError()) {       \
            (logger).log((level), (msg));        \
        }

    #define CHECK_OPENAL_LOG_DETAIL(logger, level, msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
        do {                                                                                       \
            ALenum _fife_err = alGetError();                                                        \
            if (AL_NO_ERROR != _fife_err) {                                                         \
                (logger).log((level), std::format("{}" ", Error#: {}", (msg), _fife_err));          \
            }                                                                                       \
        } while (0)

#else

    #define CHECK_OPENAL_LOG(logger, level, msg)
    #define CHECK_OPENAL_LOG_DETAIL(logger, level, msg)

#endif // LOG_ENABLED
// clang-format on

#define CHECK_OPENAL_EXCEPTION(msg) /* NOLINT(cppcoreguidelines-macro-usage) */ \
    if (AL_NO_ERROR != alGetError()) {                                          \
        throw Exception((msg));                                                 \
    }

#endif // FIFE_FIFE_OPENAL_H
