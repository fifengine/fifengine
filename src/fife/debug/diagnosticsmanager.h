// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_DIAGNOSTICS_MANAGER_H
#define FIFE_DEBUG_DIAGNOSTICS_MANAGER_H

#include <memory>

#include "debug/diagnosticsconfig.h"
#include "debug/renderdoc/renderdocdebugger.h"
#include "debug/tracy/tracygpuprofiler.h"
#include "debug/tracy/tracymemoryprofiler.h"
#include "debug/tracy/tracyprofiler.h"
#include "platform.h"

struct SDL_Window;

namespace FIFE
{
    /** Single owner of FIFE's optional debugging tools.
     *
     * RenderDoc and Tracy are kept independent: neither can start the other's
     * work. Engine calls only this manager; tool APIs remain reachable through
     * the accessors for hosts that need them.
     */
    class FIFE_API DiagnosticsManager
    {
        public:
            DiagnosticsManager();
            ~DiagnosticsManager();
            DiagnosticsManager(DiagnosticsManager const &)            = delete;
            DiagnosticsManager& operator=(DiagnosticsManager const &) = delete;

            /** @return true when at least one tool was enabled and initialized. */
            bool Initialize(DiagnosticsConfig const & config, bool hasGLContext, SDL_Window* window);

            /** Idempotent; keeps the facades alive for re-initialization. */
            void Shutdown();

            bool IsProfilingEnabled() const;
            bool IsGraphicsDebuggingEnabled() const;

            RenderDocDebugger* GetGraphicsDebugger() const
            {
                return m_graphics.get();
            }

            TracyProfiler* GetProfiler() const
            {
                return m_profiler.get();
            }

            TracyGPUProfiler* GetGPUProfiler() const
            {
                return m_gpu.get();
            }

            TracyMemoryProfiler* GetMemoryProfiler() const
            {
                return m_memory.get();
            }

            /** Called after the engine makes its GL context current. */
            void OnGLContextCurrent();

            void OnFrameBegin();
            void OnFrameEnd();

            /** Starts a RenderDoc debug group; call EndPhase() to close it. */
            void BeginPhase(char const * name);

            void EndPhase();

        private:
            std::unique_ptr<RenderDocDebugger> m_graphics;
            std::unique_ptr<TracyProfiler> m_profiler;
            std::unique_ptr<TracyGPUProfiler> m_gpu;
            std::unique_ptr<TracyMemoryProfiler> m_memory;
            DiagnosticsConfig m_config{};
            bool m_initialized{false};
    };
} // namespace FIFE

#endif
