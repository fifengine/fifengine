// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/renderdoc/renderdocdebugger.h"

#include <SDL3/SDL_video.h>

#include <bit>
#include <cstdint>
#include <string>

namespace FIFE
{
    namespace
    {
        std::string pathTemplateFor(std::string const & directory)
        {
            if (directory.empty()) {
                return {};
            }
            if (directory.back() == '/' || directory.back() == '\\') {
                return directory + "fife_capture";
            }
            return directory + "/fife_capture";
        }
    } // namespace

    RenderDocDebugger::RenderDocDebugger() : m_capture(m_api), m_markers(m_api)
    {
    }

    RenderDocDebugger::RenderDocDebugger(IRenderDocLibraryLoader* loader) :
        m_api(loader), m_capture(m_api), m_markers(m_api)
    {
    }

    bool RenderDocDebugger::initialize(RenderDocConfig const & config)
    {
        shutdown();
        if (!config.enableRenderDoc) {
            // Silent by design: a disabled module must not change startup output.
            return false;
        }
        if (m_api.initialize() != RenderDocAPI::InitResult::Success) {
            return false;
        }
        m_config = config;
        m_capture.setLogEvents(config.logCaptureEvents);
        m_markers.setEnabled(config.enableDebugMarkers);
        if (!config.captureDirectory.empty()) {
            m_capture.setCapturePath(pathTemplateFor(config.captureDirectory));
        }
        m_autoCapturePending = config.autoCapture;
        m_enabled            = true;
        return true;
    }

    void RenderDocDebugger::shutdown()
    {
        m_markers.setContextValid(false);
        m_markers.reset();
        m_capture.reset();
        m_enabled            = false;
        m_autoCapturePending = false;
        m_api.shutdown();
    }

    void RenderDocDebugger::configureWindow(SDL_Window* window)
    {
        if (!m_enabled || window == nullptr) {
            return;
        }
        // For OpenGL the device pointer must be the native context (HGLRC /
        // GLXContext / EGLContext). SDL_GLContext is opaque, so we keep the
        // documented NULL wildcard and only pin the platform window handle.
        void* windowHandle           = nullptr;
        SDL_PropertiesID const props = SDL_GetWindowProperties(window);
#ifdef _WIN32
        windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
#elif defined(__linux__) && !defined(__ANDROID__)
        Sint64 const xid = SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
        if (xid != 0) {
            windowHandle = std::bit_cast<void*>(static_cast<uintptr_t>(xid));
        }
        if (windowHandle == nullptr) {
            windowHandle = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
        }
#endif
        m_capture.setTarget(nullptr, windowHandle);
    }

    void RenderDocDebugger::onFrameBegin()
    {
        if (!m_enabled) {
            return;
        }
        if (m_autoCapturePending) {
            m_autoCapturePending = false;
            m_capture.triggerCapture();
        }
        m_markers.beginEvent(passName(RenderDocPass::Frame));
    }

    void RenderDocDebugger::onFrameEnd()
    {
        if (!m_enabled) {
            return;
        }
        m_markers.endEvent();
    }

    void RenderDocDebugger::nameTexture(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameTexture(name, label);
        }
    }

    void RenderDocDebugger::nameFramebuffer(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameFramebuffer(name, label);
        }
    }

    void RenderDocDebugger::nameBuffer(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameBuffer(name, label);
        }
    }

    void RenderDocDebugger::nameShader(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameShader(name, label);
        }
    }

    void RenderDocDebugger::nameProgram(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameProgram(name, label);
        }
    }

    void RenderDocDebugger::nameRenderbuffer(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameRenderbuffer(name, label);
        }
    }

    void RenderDocDebugger::nameVertexArray(uint32_t name, char const * label)
    {
        if (m_enabled && m_config.enableResourceNaming) {
            m_markers.nameVertexArray(name, label);
        }
    }
} // namespace FIFE
