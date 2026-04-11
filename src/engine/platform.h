// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PLATFORM_H
#define FIFE_PLATFORM_H

#if !defined(FIFE_STATIC)
    #if defined(FIFE_STATICLIB)
        #define FIFE_STATIC 1
    #endif
#endif

#if defined(_WIN32)
    #if defined(FIFE_STATIC)
        #define FIFE_API
    #elif defined(FIFE_EXPORTING)
        #define FIFE_API __declspec(dllexport)
    #else
        #define FIFE_API __declspec(dllimport)
    #endif
#else
    #define FIFE_API __attribute__((visibility("default")))
#endif

#ifndef FIFE_API
    #define FIFE_API
#endif

#endif // FIFE_PLATFORM_H
