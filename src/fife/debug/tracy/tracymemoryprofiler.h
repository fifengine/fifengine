// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_TRACY_MEMORY_PROFILER_H
#define FIFE_DEBUG_TRACY_MEMORY_PROFILER_H

#include <cstddef>

#include "debug/diagnosticsconfig.h"
#include "platform.h"

namespace FIFE
{
    /** Named-pool allocation tracking through TracyAllocN/TracyFreeN.
     *
     * FIFE has no custom allocator, so this module reports selected buffers at
     * known sites and offers a host API. It never replaces new/delete.
     */
    class FIFE_API TracyMemoryProfiler
    {
        public:
            explicit TracyMemoryProfiler(TracyConfig const & config);
            virtual ~TracyMemoryProfiler()                              = default;
            TracyMemoryProfiler(TracyMemoryProfiler const &)            = delete;
            TracyMemoryProfiler& operator=(TracyMemoryProfiler const &) = delete;

            bool Initialize();
            void Shutdown();

            bool IsEnabled() const
            {
                return m_enabled;
            }

            virtual void TrackAllocation(void* ptr, std::size_t size, char const * poolName);
            virtual void TrackFree(void* ptr, char const * poolName);

        private:
            TracyConfig m_config;
            bool m_enabled{false};
    };

    /** RAII reporter for a single known buffer. */
    class FIFE_API TrackedAllocation
    {
        public:
            /** Standalone form used inside FIFE's resource loading. */
            TrackedAllocation(void* ptr, std::size_t size, char const * poolName);

            /** Host form that reports through @p profiler. */
            TrackedAllocation(TracyMemoryProfiler& profiler, void* ptr, std::size_t size, char const * poolName);

            ~TrackedAllocation();
            TrackedAllocation(TrackedAllocation const &)            = delete;
            TrackedAllocation& operator=(TrackedAllocation const &) = delete;

        private:
            TracyMemoryProfiler* m_profiler{nullptr};
            void* m_ptr{nullptr};
            char const * m_pool{nullptr};
    };
} // namespace FIFE

#endif
