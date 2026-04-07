// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EXPORT_H
#define FIFE_EXPORT_H

#if defined(_WIN32)

    #if defined(FIFE_EXPORTING)
        #define FIFE_PUBLIC __declspec(dllexport)
    #else
        #define FIFE_PUBLIC __declspec(dllimport)
    #endif

    #define FIFE_HIDDEN

#else

    #if defined(__GNUC__) && __GNUC__ >= 4
        #define FIFE_PUBLIC __attribute__((visibility("default")))
        #define FIFE_HIDDEN __attribute__((visibility("hidden")))
    #else
        #define FIFE_PUBLIC
        #define FIFE_HIDDEN
    #endif

#endif

#endif // FIFE_EXPORT_H
