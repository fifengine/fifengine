// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/tracy/tracymemoryprofiler.h"

#include "util/log/logger.h"

#if defined(FIFE_ENABLE_TRACY)
    #include <tracy/Tracy.hpp>
#endif

namespace FIFE
{
#if !defined(FIFE_ENABLE_TRACY)
    namespace
    {
        Logger& _log()
        {
            static Logger log(LM_VIDEO);
            return log;
        }
    } // namespace
#endif

    namespace
    {
        void reportAllocation(void* ptr, std::size_t size, char const * poolName)
        {
            if (ptr == nullptr || poolName == nullptr) {
                return;
            }
#if defined(FIFE_ENABLE_TRACY)
            TracyAllocN(ptr, size, poolName);
#else
            (void)ptr;
            (void)size;
            (void)poolName;
#endif
        }

        void reportFree(void* ptr, char const * poolName)
        {
            if (ptr == nullptr || poolName == nullptr) {
                return;
            }
#if defined(FIFE_ENABLE_TRACY)
            TracyFreeN(ptr, poolName);
#else
            (void)ptr;
            (void)poolName;
#endif
        }
    } // namespace

    TracyMemoryProfiler::TracyMemoryProfiler(TracyConfig const & config) : m_config(config)
    {
    }

    bool TracyMemoryProfiler::Initialize()
    {
        Shutdown();
        if (!m_config.enableTracy || !m_config.enableMemoryProfiling) {
            return false;
        }
#if defined(FIFE_ENABLE_TRACY)
        m_enabled = true;
#else
        FL_WARN(_log(), "Tracy memory tracking requested but this build was compiled with ENABLE_TRACY=OFF");
#endif
        return m_enabled;
    }

    void TracyMemoryProfiler::Shutdown()
    {
        m_enabled = false;
    }

    void TracyMemoryProfiler::TrackAllocation(void* ptr, std::size_t size, char const * poolName)
    {
        if (!m_enabled) {
            return;
        }
        reportAllocation(ptr, size, poolName);
    }

    void TracyMemoryProfiler::TrackFree(void* ptr, char const * poolName)
    {
        if (!m_enabled) {
            return;
        }
        reportFree(ptr, poolName);
    }

    TrackedAllocation::TrackedAllocation(void* ptr, std::size_t size, char const * poolName) :
        m_ptr(ptr), m_pool(poolName)
    {
        reportAllocation(m_ptr, size, m_pool);
    }

    TrackedAllocation::TrackedAllocation(
        TracyMemoryProfiler& profiler, void* ptr, std::size_t size, char const * poolName) :
        m_profiler(&profiler), m_ptr(ptr), m_pool(poolName)
    {
        profiler.TrackAllocation(ptr, size, poolName);
    }

    TrackedAllocation::~TrackedAllocation()
    {
        if (m_profiler != nullptr) {
            m_profiler->TrackFree(m_ptr, m_pool);
        } else {
            reportFree(m_ptr, m_pool);
        }
    }
} // namespace FIFE
