// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/tracy/tracyprofiler.h"

#include <cstring>

#include "tracyprofilermacros.h"
#include "util/log/logger.h"

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

    TracyProfiler::TracyProfiler(TracyConfig const & config) : m_config(config)
    {
    }

    TracyProfiler::~TracyProfiler()
    {
        Shutdown();
    }

    bool TracyProfiler::Initialize()
    {
        Shutdown();
        if (!m_config.enableTracy) {
            return false;
        }
#ifdef FIFE_ENABLE_TRACY
        m_enabled = true;
        FL_LOG(_log(), "Tracy CPU profiler initialized (on-demand; zones inactive until a viewer connects)");
#else
        FL_WARN(_log(), "Tracy profiling requested but this build was compiled with ENABLE_TRACY=OFF");
        m_enabled = false;
#endif
        return m_enabled;
    }

    void TracyProfiler::Shutdown()
    {
        m_enabled = false;
    }

    bool TracyProfiler::IsProfilerConnected() const
    {
#ifdef FIFE_ENABLE_TRACY
        return m_enabled && tracy::GetProfiler().IsConnected();
#else
        return false;
#endif
    }

    void TracyProfiler::MarkFrame() const
    {
        if (!m_enabled) {
            return;
        }
        FIFE_PROFILE_FRAME_MARK();
    }

    void TracyProfiler::SetPhaseText(char const * text) const
    {
        if (!m_enabled || text == nullptr) {
            return;
        }
#ifdef FIFE_ENABLE_TRACY
        TracyMessageL(text);
#else
        (void)text;
#endif
    }
} // namespace FIFE
