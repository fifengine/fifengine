// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_TRACY_GPU_PROFILER_H
#define FIFE_DEBUG_TRACY_GPU_PROFILER_H

#include <memory>

#include "debug/diagnosticsconfig.h"
#include "platform.h"

namespace FIFE
{
    /** Official Tracy OpenGL GPU profiling integration.
     *
     * Initialization requires a current context on this thread and a driver
     * that implements timestamp queries. If either check fails, every method
     * is a no-op — Tracy's GpuCtx asserts on unsupported drivers, so FIFE must
     * never invoke the macro without these checks.
     */
    class FIFE_API TracyGPUProfiler
    {
        public:
            explicit TracyGPUProfiler(TracyConfig const & config);
            ~TracyGPUProfiler();
            TracyGPUProfiler(TracyGPUProfiler const &)            = delete;
            TracyGPUProfiler& operator=(TracyGPUProfiler const &) = delete;

            bool Initialize(bool contextCurrent);
            void Shutdown();

            bool IsEnabled() const
            {
                return m_enabled;
            }

            void BeginPhase(char const * name);
            void Collect();

            /** Checks the timer-query extension and GL_QUERY_COUNTER_BITS.
             *  Must only be called with a current context. */
            static bool HasTimerQuerySupport();

        private:
            TracyConfig m_config;
            bool m_enabled{false};
    };

    /** RAII wrapper for a real Tracy GPU zone. The zone spans this object's
     * lifetime, so it must be constructed inside the phase it measures. */
    class FIFE_API TracyGPUPhase
    {
        public:
            TracyGPUPhase(TracyGPUProfiler& profiler, char const * name);
            ~TracyGPUPhase();
            TracyGPUPhase(TracyGPUPhase const &)            = delete;
            TracyGPUPhase& operator=(TracyGPUPhase const &) = delete;

        private:
            struct Impl;
            std::unique_ptr<Impl> m_impl;
    };
} // namespace FIFE

#endif
