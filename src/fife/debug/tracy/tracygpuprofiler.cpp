// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/tracy/tracygpuprofiler.h"

#include <cstring>
#include <memory>

#include "util/log/logger.h"

// Tracy's OpenGL backend issues raw GL calls, so it must be included after
// FIFE's vendored glad header (which supplies the GL tokens and entry points).
#if defined(FIFE_ENABLE_TRACY) && \
    (defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_ES2) || defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31))
    #include "video/glcore/glad/glcore_glad.h"
    #include <tracy/TracyOpenGL.hpp> // NOLINT(build/include_alpha): must follow glad
    #define FIFE_TRACY_GL_AVAILABLE 1
#endif

namespace FIFE
{
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }
    } // namespace

    TracyGPUProfiler::TracyGPUProfiler(TracyConfig const & config) : m_config(config)
    {
    }

    TracyGPUProfiler::~TracyGPUProfiler()
    {
        Shutdown();
    }

    bool TracyGPUProfiler::HasTimerQuerySupport()
    {
#ifdef FIFE_TRACY_GL_AVAILABLE
        bool extension = false;
    #if defined(GL_VERSION_3_0) || defined(GL_VERSION_3_1) || defined(GL_VERSION_4_3)
        extension = GLAD_GL_ARB_timer_query != 0;
    #else
        extension = GLAD_GL_EXT_disjoint_timer_query != 0;
    #endif
        if (!extension) {
            return false;
        }
        GLint bits = 0;
    #if defined(GL_TIMESTAMP)
        glGetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &bits);
    #else
        glGetQueryiv(GL_TIMESTAMP_EXT, GL_QUERY_COUNTER_BITS_EXT, &bits);
    #endif
        return bits > 0 && glGetError() == GL_NO_ERROR;
#else
        return false;
#endif
    }

    bool TracyGPUProfiler::Initialize(bool contextCurrent)
    {
        Shutdown();
        if (!m_config.enableTracy || !m_config.enableGPUProfiling || !contextCurrent) {
            return false;
        }
        if (!HasTimerQuerySupport()) {
            FL_WARN(_log(), "Tracy GPU profiling unavailable: no GL timestamp query support");
            return false;
        }
#ifdef FIFE_TRACY_GL_AVAILABLE
        TracyGpuContext;
        TracyGpuContextName("FIFE OpenGL", sizeof("FIFE OpenGL") - 1);
        m_enabled = true;
        FL_LOG(_log(), "Tracy OpenGL GPU profiling initialized");
#else
        FL_WARN(_log(), "Tracy GPU profiling requested but this build has no supported GL backend or Tracy support");
#endif
        return m_enabled;
    }

    void TracyGPUProfiler::Shutdown()
    {
        // Tracy's OpenGL backend has no destroy macro; the context is owned by
        // the engine and must outlive all zones.
        m_enabled = false;
    }

    void TracyGPUProfiler::BeginPhase(char const * name) const
    {
        if (!m_enabled || name == nullptr) {
            return;
        }
#ifdef FIFE_TRACY_GL_AVAILABLE
        // Transient variant accepts a runtime name (constexpr name variables
        // cannot initialise TracyGpuZone's static SourceLocationData).
        TracyGpuZoneTransient(__tracy_phase_gpu_zone, name, true);
#endif
    }

    void TracyGPUProfiler::Collect() const
    {
        if (!m_enabled) {
            return;
        }
#ifdef FIFE_TRACY_GL_AVAILABLE
        TracyGpuCollect;
#endif
    }

#ifdef FIFE_TRACY_GL_AVAILABLE
    struct TracyGPUPhase::Impl
    {
            explicit Impl(char const * name) :
                scope(
                    static_cast<uint32_t>(TracyLine),
                    TracyFile,
                    std::strlen(TracyFile),
                    TracyFunction,
                    std::strlen(TracyFunction),
                    name,
                    std::strlen(name),
                    true)
            {
            }

            tracy::GpuCtxScope scope;
    };
#else
    struct TracyGPUPhase::Impl
    {
    };
#endif

    TracyGPUPhase::TracyGPUPhase(TracyGPUProfiler& profiler, char const * name)
    {
        if (!profiler.IsEnabled() || name == nullptr) {
            return;
        }
#ifdef FIFE_TRACY_GL_AVAILABLE
        m_impl = std::make_unique<Impl>(name);
#endif
    }

    TracyGPUPhase::~TracyGPUPhase() = default;
} // namespace FIFE
