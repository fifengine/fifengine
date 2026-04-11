// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PLATFORM_H
#define FIFE_PLATFORM_H

#if !defined(FIFE_STATIC)
    #if defined(FIFE_STATICLIB)
        #define FIFE_STATIC 1
    #endif
#endif

/**
 * - When SWIG is used, we set FIFE_API to empty, because we are generating
 *   bindings and don't want to export symbols. Also (it seems) Swig doesn't
 *   support __declspec(dllexport) or __declspec(dllimport) it properly.
 */
#if defined(_WIN32)
    #if defined(FIFE_STATIC)
        #define FIFE_API
    #elif defined(FIFE_EXPORTING)
        #define FIFE_API __declspec(dllexport)
    #elif defined(SWIG)
        #define FIFE_API
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
