// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/diagnosticsmanager.h"

#include <SDL3/SDL_video.h>

#include <memory>

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

    DiagnosticsManager::DiagnosticsManager()
    {
        // The RenderDoc facade is always present after construction so hosts
        // can keep the existing non-null accessor contract.
        m_graphics = std::make_unique<RenderDocDebugger>();
        m_profiler = std::make_unique<TracyProfiler>(m_config.tracy);
        m_gpu      = std::make_unique<TracyGPUProfiler>(m_config.tracy);
        m_memory   = std::make_unique<TracyMemoryProfiler>(m_config.tracy);
    }

    DiagnosticsManager::~DiagnosticsManager()
    {
        Shutdown();
    }

    bool DiagnosticsManager::Initialize(DiagnosticsConfig const & config, bool hasGLContext, SDL_Window* window)
    {
        Shutdown();
        m_config = config;

        // Each tool receives only its own config; neither observes the other.
        m_graphics                 = std::make_unique<RenderDocDebugger>();
        bool const graphicsEnabled = m_graphics->initialize(m_config.renderDoc);
        if (graphicsEnabled) {
            m_graphics->setContextValid(hasGLContext);
            if (window != nullptr) {
                m_graphics->configureWindow(window);
            }
        }

        m_profiler                  = std::make_unique<TracyProfiler>(m_config.tracy);
        m_gpu                       = std::make_unique<TracyGPUProfiler>(m_config.tracy);
        m_memory                    = std::make_unique<TracyMemoryProfiler>(m_config.tracy);
        bool const profilingEnabled = m_profiler->Initialize();
        m_memory->Initialize();
        if (hasGLContext) {
            m_gpu->Initialize(true);
        }

        m_initialized = graphicsEnabled || profilingEnabled;
        if (m_initialized) {
            FL_LOG(_log(), "Diagnostics initialized");
        }
        return m_initialized;
    }

    void DiagnosticsManager::Shutdown()
    {
        if (m_gpu) {
            m_gpu->Shutdown();
        }
        if (m_memory) {
            m_memory->Shutdown();
        }
        if (m_profiler) {
            m_profiler->Shutdown();
        }
        if (m_graphics) {
            m_graphics->shutdown();
        }
        m_initialized = false;
    }

    bool DiagnosticsManager::IsProfilingEnabled() const
    {
        return m_initialized && m_profiler != nullptr && m_profiler->IsEnabled();
    }

    bool DiagnosticsManager::IsGraphicsDebuggingEnabled() const
    {
        return m_initialized && m_graphics != nullptr && m_graphics->isEnabled();
    }

    void DiagnosticsManager::OnGLContextCurrent()
    {
        if (!m_initialized) {
            return;
        }
        if (m_graphics->isEnabled()) {
            m_graphics->setContextValid(true);
        }
        if (m_gpu && !m_gpu->IsEnabled()) {
            m_gpu->Initialize(true);
        }
    }

    void DiagnosticsManager::OnFrameBegin()
    {
        if (IsGraphicsDebuggingEnabled()) {
            m_graphics->onFrameBegin();
        }
    }

    void DiagnosticsManager::OnFrameEnd()
    {
        if (IsGraphicsDebuggingEnabled()) {
            m_graphics->onFrameEnd();
        }
        if (IsProfilingEnabled()) {
            m_profiler->MarkFrame();
        }
        if (m_gpu && m_gpu->IsEnabled()) {
            m_gpu->Collect();
        }
    }

    void DiagnosticsManager::BeginPhase(char const * name)
    {
        if (IsGraphicsDebuggingEnabled()) {
            m_graphics->beginEvent(name);
        }
    }

    void DiagnosticsManager::EndPhase()
    {
        if (IsGraphicsDebuggingEnabled()) {
            m_graphics->endEvent();
        }
    }
} // namespace FIFE
