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
 * When SWIG parses the interface file, %define FIFE_API %enddef in fife.i
 * tells the SWIG preprocessor to ignore the macro. In the generated C++
 * wrapper, FIFE_API resolves to __declspec(dllimport) on Windows or
 * __attribute__((visibility("default"))) on Linux/macOS so that the SWIG
 * module correctly imports symbols from the fifengine shared library.
 */
#if defined(_WIN32)
    // #pragma message(">>> SETTING: Taking Windows branch")
    #if defined(FIFE_STATIC)
        // #pragma message(">>> SETTING: FIFE_STATIC")
        #define FIFE_API
    #elif defined(FIFE_BUILD) || defined(fife_EXPORTS)
        // #pragma message(">>> SETTING: FIFE_BUILD/fife_EXPORTS")
        #define FIFE_API __declspec(dllexport)
    #else
        // #pragma message(">>> SETTING: Windows DLL import")
        #define FIFE_API __declspec(dllimport)
    #endif
#elif defined(__linux__) || defined(__APPLE__)
    // For Linux/macOS, we only need to use __attribute__ for visibility.
    // #pragma message(">>> SETTING: Taking Linux/macOS branch")
    #if defined(FIFE_BUILD) || defined(fife_EXPORTS)
        // #pragma message(">>> SETTING: FIFE_BUILD/fife_EXPORTS")
        #define FIFE_API __attribute__((visibility("default")))
    #elif defined(FIFE_DLL_IMPORT)
        // #pragma message(">>> SETTING: FIFE_DLL_IMPORT")
        #define FIFE_API __attribute__((visibility("default")))
    #else
        // #pragma message(">>> SETTING: Linux/macOS default")
        #define FIFE_API __attribute__((visibility("default")))
    #endif
#else
    // #pragma message(">>> SETTING: Taking unknown branch")
    #define FIFE_API
#endif

#endif // FIFE_PLATFORM_H
