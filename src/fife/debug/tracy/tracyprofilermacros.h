// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_TRACY_PROFILER_MACROS_H
#define FIFE_DEBUG_TRACY_PROFILER_MACROS_H

// Tracy's macros are process-global and need no profiler object. When Tracy is
// not compiled in they expand to empty statements, so instrumented call sites
// cost nothing and pull in no Tracy headers.
#if defined(FIFE_ENABLE_TRACY)
    #include <tracy/Tracy.hpp>
    #define FIFE_PROFILE_ZONE()       ZoneScoped
    #define FIFE_PROFILE_ZONE_N(name) ZoneScopedN(name)
    #define FIFE_PROFILE_FRAME_MARK() FrameMark
#else
    #define FIFE_PROFILE_ZONE()       ((void)0)
    #define FIFE_PROFILE_ZONE_N(name) ((void)(name))
    #define FIFE_PROFILE_FRAME_MARK() ((void)0)
#endif

#endif
