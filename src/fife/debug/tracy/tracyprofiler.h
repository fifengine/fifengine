// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_TRACY_PROFILER_H
#define FIFE_DEBUG_TRACY_PROFILER_H

#include "debug/diagnosticsconfig.h"
#include "platform.h"

namespace FIFE
{
    /** CPU-side Tracy integration: connection state, frame marks and messages.
     *
     * Actual CPU zones use the standalone macros in tracyprofilermacros.h so
     * instrumented call sites need no object and no lifetime rules.
     */
    class FIFE_API TracyProfiler
    {
        public:
            explicit TracyProfiler(TracyConfig const & config);
            ~TracyProfiler();
            TracyProfiler(TracyProfiler const &)            = delete;
            TracyProfiler& operator=(TracyProfiler const &) = delete;

            bool Initialize();
            void Shutdown();

            /** True when compiled with Tracy and enabled by config. */
            bool IsEnabled() const
            {
                return m_enabled;
            }

            /** True only when a Tracy viewer is currently connected. */
            bool IsProfilerConnected() const;

            /** Emits Tracy's end-of-frame marker (named MarkFrame to avoid the FrameMark macro). */
            void MarkFrame() const;

            /** Emits a one-shot message on the currently open zone. */
            void SetPhaseText(char const * text) const;

        private:
            TracyConfig m_config;
            bool m_enabled{false};
    };
} // namespace FIFE

#endif
