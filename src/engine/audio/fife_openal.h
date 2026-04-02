// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_FIFE_OPENAL_H
#define FIFE_FIFE_OPENAL_H

#define AL_ALEXT_PROTOTYPES

// Standard C++ library includes

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx-presets.h>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

#ifdef LOG_ENABLED

    #define CHECK_OPENAL_LOG(logger, level, msg) \
        if (AL_NO_ERROR != alGetError()) {       \
            (logger).log((level), (msg));        \
        }

    #define CHECK_OPENAL_LOG_DETAIL(logger, level, msg)                      \
        {                                                                    \
            ALenum error;                                                    \
            error = alGetError();                                            \
            if (AL_NO_ERROR != error) {                                      \
                (logger).log((level), LMsg() << (msg) << ", Error#: " << error); \
            }                                                                \
        }

#else

    #define CHECK_OPENAL_LOG(logger, level, msg)
    #define CHECK_OPENAL_LOG_DETAIL(logger, level, msg)

#endif

#define CHECK_OPENAL_EXCEPTION(msg)    \
    if (AL_NO_ERROR != alGetError()) { \
        throw Exception((msg));        \
    }

#endif
