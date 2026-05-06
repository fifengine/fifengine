// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PLATFORM_H
#define FIFE_PLATFORM_H

// Check and defines for the operating system
#if !defined(FIFE_OS_WINDOWS) && (defined(_WIN32) || defined(_WIN64))
    #define FIFE_OS_WINDOWS 1
#endif
#if !defined(FIFE_OS_LINUX) && defined(__linux__)
    #define FIFE_OS_LINUX 1
#endif
#if !defined(FIFE_OS_MACOS) && defined(__APPLE__)
    #define FIFE_OS_MACOS 1
#endif

// Check and defines for the compiler
#if !defined(FIFE_COMPILER_MSVC) && defined(_MSC_VER)
    #define FIFE_COMPILER_MSVC 1
#endif
#if !defined(FIFE_COMPILER_GNU) && defined(__GNUC__)
    #define FIFE_COMPILER_GNU 1
#endif
#if !defined(FIFE_COMPILER_CLANG) && defined(__clang__)
    #define FIFE_COMPILER_CLANG 1
#endif
#if !defined(FIFE_COMPILER_MINGW) && defined(__MINGW32__)
    #define FIFE_COMPILER_MINGW 1
#endif

// Static builds do not use import/export decorations on Windows.
#if !defined(FIFE_STATIC)
    #if defined(FIFE_STATICLIB)
        #define FIFE_STATIC 1
    #endif
#endif

/**
 * Symbol Visibility
 *
 * When SWIG is used, we set FIFE_API to empty, because we are generating
 * bindings and don't want to export symbols. Also (it seems) Swig doesn't
 * support __declspec(dllexport) or __declspec(dllimport) properly.
 */
#if defined(_WIN32)
    #if defined(FIFE_STATIC)
        #define FIFE_API
    #elif defined(FIFE_BUILD) || defined(fife_EXPORTS)
        #define FIFE_API __declspec(dllexport)
    #elif defined(SWIG)
        #define FIFE_API
    #else
        #define FIFE_API __declspec(dllimport)
    #endif
#elif defined(__linux__) || defined(__APPLE__)
    // For Linux/macOS, we only need to use __attribute__ for visibility.
    #if defined(FIFE_BUILD) || defined(fife_EXPORTS)
        #define FIFE_API __attribute__((visibility("default")))
    #elif defined(FIFE_DLL_IMPORT)
        #define FIFE_API __attribute__((visibility("default")))
    #else
        #define FIFE_API __attribute__((visibility("default")))
    #endif
#else
    #define FIFE_API
#endif

#endif // FIFE_PLATFORM_H
