// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <string>

#include "controller/enginesettings.h"
#include "debug/diagnosticsconfig.h"
#include "debug/diagnosticsphases.h"
#include "debug/renderdoc/renderdocmarkers.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha): clang-format sorts by group

TEST_CASE("diagnostics configs use safe defaults", "[diagnostics]")
{
    FIFE::DiagnosticsConfig config;
    CHECK_FALSE(config.renderDoc.enableRenderDoc);
    CHECK_FALSE(config.tracy.enableTracy);
    CHECK(config.tracy.enableGPUProfiling);
    CHECK(config.tracy.enableMemoryProfiling);
    CHECK(config.renderDoc.captureHotkey == "F12");
}

TEST_CASE("engine settings round-trip Tracy options", "[diagnostics]")
{
    FIFE::EngineSettings settings;
    CHECK_FALSE(settings.isTracyEnabled());
    CHECK(settings.isTracyGPUProfiling());
    CHECK(settings.isTracyMemoryProfiling());

    settings.setTracyEnabled(true);
    settings.setTracyGPUProfiling(false);
    settings.setTracyMemoryProfiling(false);

    CHECK(settings.isTracyEnabled());
    CHECK_FALSE(settings.isTracyGPUProfiling());
    CHECK_FALSE(settings.isTracyMemoryProfiling());
}

TEST_CASE("diagnostic phase names match RenderDoc pass names", "[diagnostics]")
{
    CHECK(std::string(FIFE::kPhaseFrame) == FIFE::passName(FIFE::RenderDocPass::Frame));
    CHECK(std::string(FIFE::kPhaseOffscreen) == FIFE::passName(FIFE::RenderDocPass::Offscreen));
    CHECK(std::string(FIFE::kPhaseTarget) == FIFE::passName(FIFE::RenderDocPass::Target));
    CHECK(std::string(FIFE::kPhaseWorld) == FIFE::passName(FIFE::RenderDocPass::World));
    CHECK(std::string(FIFE::kPhaseGui) == FIFE::passName(FIFE::RenderDocPass::Gui));
    CHECK(std::string(FIFE::kPhaseCursor) == FIFE::passName(FIFE::RenderDocPass::Cursor));
    CHECK(std::string(FIFE::kPhaseMovie) == FIFE::passName(FIFE::RenderDocPass::Movie));
}

#include "debug/tracy/tracyprofiler.h"

TEST_CASE("tracy profiler is disabled by default and safe to shut down", "[diagnostics]")
{
    FIFE::TracyConfig config;
    FIFE::TracyProfiler profiler(config);
    CHECK_FALSE(profiler.IsEnabled());
    CHECK_FALSE(profiler.Initialize());
    profiler.MarkFrame();
    profiler.SetPhaseText("Frame");
    profiler.Shutdown();
    profiler.Shutdown();
    CHECK_FALSE(profiler.IsEnabled());
    CHECK_FALSE(profiler.IsProfilerConnected());
}

TEST_CASE("tracy profiler reports unavailable when compiled out", "[diagnostics]")
{
    FIFE::TracyConfig config;
    config.enableTracy = true;
    FIFE::TracyProfiler profiler(config);
#if defined(FIFE_ENABLE_TRACY)
    CHECK(profiler.Initialize());
    CHECK(profiler.IsEnabled());
#else
    CHECK_FALSE(profiler.Initialize());
    CHECK_FALSE(profiler.IsEnabled());
#endif
    profiler.Shutdown();
}

#include "debug/tracy/tracygpuprofiler.h"

TEST_CASE("tracy GPU profiler is inert without a current GL context", "[diagnostics]")
{
    FIFE::TracyConfig config;
    config.enableTracy        = true;
    config.enableGPUProfiling = true;
    FIFE::TracyGPUProfiler gpu(config);

    CHECK_FALSE(gpu.Initialize(false));
    CHECK_FALSE(gpu.IsEnabled());
    gpu.BeginPhase(FIFE::kPhaseTarget);
    gpu.BeginPhase(nullptr);
    gpu.Collect();
    CHECK_FALSE(gpu.IsEnabled());
}

TEST_CASE("tracy GPU profiler shuts down idempotently", "[diagnostics]")
{
    FIFE::TracyConfig config;
    config.enableTracy        = true;
    config.enableGPUProfiling = true;
    FIFE::TracyGPUProfiler gpu(config);
#if !defined(FIFE_ENABLE_TRACY)
    CHECK_FALSE(gpu.Initialize(true));
#endif
    gpu.Shutdown();
    gpu.Shutdown();
    CHECK_FALSE(gpu.IsEnabled());
}

TEST_CASE("tracy GPU profiling can be disabled by config", "[diagnostics]")
{
    FIFE::TracyConfig config;
    config.enableTracy        = true;
    config.enableGPUProfiling = false;
    FIFE::TracyGPUProfiler gpu(config);
    CHECK_FALSE(gpu.Initialize(true));
    CHECK_FALSE(gpu.IsEnabled());
}

#include <cstddef>

#include "debug/tracy/tracymemoryprofiler.h"

namespace
{
    class CountingMemoryProfiler final : public FIFE::TracyMemoryProfiler
    {
        public:
            explicit CountingMemoryProfiler(FIFE::TracyConfig const & config) : TracyMemoryProfiler(config)
            {
            }

            void TrackAllocation(void* ptr, std::size_t size, char const * poolName) override
            {
                ++allocations;
                lastPtr  = ptr;
                lastSize = size;
                lastName = poolName;
            }

            void TrackFree(void* ptr, char const * poolName) override
            {
                ++frees;
                freedPtr  = ptr;
                freedName = poolName;
            }

            int allocations{0};
            int frees{0};
            void* lastPtr{nullptr};
            void* freedPtr{nullptr};
            std::size_t lastSize{0};
            char const * lastName{nullptr};
            char const * freedName{nullptr};
    };
} // namespace

TEST_CASE("tracy memory profiler is inert when disabled", "[diagnostics]")
{
    FIFE::TracyConfig config;
    FIFE::TracyMemoryProfiler profiler(config);
    CHECK_FALSE(profiler.Initialize());
    int value = 0;
    profiler.TrackAllocation(&value, sizeof(value), "test pool");
    profiler.TrackFree(&value, "test pool");
    profiler.Shutdown();
    CHECK_FALSE(profiler.IsEnabled());
}

TEST_CASE("tracked allocation guard reports matching allocation and free", "[diagnostics]")
{
    FIFE::TracyConfig config;
    CountingMemoryProfiler profiler(config);
    int value = 0;
    {
        FIFE::TrackedAllocation guard(profiler, &value, sizeof(value), "test pool");
        CHECK(profiler.allocations == 1);
        CHECK(profiler.frees == 0);
    }
    CHECK(profiler.frees == 1);
    CHECK(profiler.freedPtr == &value);
}

#include "debug/diagnosticsmanager.h"
TEST_CASE("diagnostics manager keeps tools independent", "[diagnostics]")
{
    FIFE::DiagnosticsConfig config;
    FIFE::DiagnosticsManager manager;
    CHECK(manager.Initialize(config, false, nullptr) == false);
    CHECK_FALSE(manager.IsProfilingEnabled());
    CHECK_FALSE(manager.IsGraphicsDebuggingEnabled());
    CHECK(manager.GetGraphicsDebugger() != nullptr);
    CHECK(manager.GetProfiler() != nullptr);
    CHECK(manager.GetGPUProfiler() != nullptr);
    manager.OnFrameBegin();
    manager.OnFrameEnd();
    manager.Shutdown();
}

TEST_CASE("diagnostics manager shutdown and reinitialization are safe", "[diagnostics]")
{
    FIFE::DiagnosticsConfig config;
    config.tracy.enableTracy        = true;
    config.tracy.enableGPUProfiling = false;
    FIFE::DiagnosticsManager manager;
    manager.Initialize(config, false, nullptr);
    manager.Shutdown();
    manager.Shutdown();
    manager.Initialize(config, false, nullptr);
#if defined(FIFE_ENABLE_TRACY)
    CHECK(manager.IsProfilingEnabled());
#else
    CHECK_FALSE(manager.IsProfilingEnabled());
#endif
    manager.Shutdown();
}

TEST_CASE("tracy GPU phase is inert when profiling is disabled", "[diagnostics]")
{
    FIFE::TracyConfig config;
    FIFE::TracyGPUProfiler gpu(config);
    {
        FIFE::TracyGPUPhase phase(gpu, FIFE::kPhaseTarget);
        CHECK_FALSE(gpu.IsEnabled());
    }
}
