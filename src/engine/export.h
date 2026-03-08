// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_EXPORT_H
#define FIFE_EXPORT_H

#if defined _WIN32 || defined __CYGWIN__
    #ifdef FIFE_EXPORTING
        #ifdef __GNUC__
            #define FIFE_PUBLIC __attribute__((dllexport))
        #else
            #define FIFE_PUBLIC __declspec(dllexport)
        #endif
    #else
        #ifdef __GNUC__
            #define FIFE_PUBLIC __attribute__((dllimport))
        #else
            #define FIFE_PUBLIC __declspec(dllimport)
        #endif
    #endif
    #define FIFE_HIDDEN
#else
    #if __GNUC__ >= 4
        #define FIFE_PUBLIC __attribute__((visibility("default")))
        #define FIFE_HIDDEN __attribute__((visibility("hidden")))
    #else
        #define FIFE_PUBLIC
        #define FIFE_HIDDEN
    #endif
#endif

#endif // FIFE_EXPORT_H