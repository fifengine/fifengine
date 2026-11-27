// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_RENDERDOC_DEBUGGER_H
#define FIFE_DEBUG_RENDERDOC_DEBUGGER_H

#include <string>

#include "debug/renderdoc/renderdocapi.h"
#include "debug/renderdoc/renderdoccapture.h"
#include "debug/renderdoc/renderdocconfig.h"
#include "debug/renderdoc/renderdocmarkers.h"
#include "platform.h"

struct SDL_Window;

namespace FIFE
{
    /** Public facade for the standalone RenderDoc debugger module.
     *
     * Owned by Engine. All methods are safe no-ops when RenderDoc is absent or
     * the module is disabled; no GL function is called without a valid context.
     */
    class FIFE_API RenderDocDebugger
    {
        public:
            RenderDocDebugger();
            explicit RenderDocDebugger(IRenderDocLibraryLoader* loader);

            RenderDocDebugger(RenderDocDebugger const &)            = delete;
            RenderDocDebugger& operator=(RenderDocDebugger const &) = delete;

            /** Loads the API and applies configuration. Returns false when disabled/unavailable. */
            bool initialize(RenderDocConfig const & config);

            /** Idempotent teardown. */
            void shutdown();

            bool isAvailable() const
            {
                return m_api.isAvailable();
            }

            bool isInitialized() const
            {
                return m_enabled && m_api.isInitialized();
            }

            bool isEnabled() const
            {
                return m_enabled;
            }

            RenderDocConfig const & config() const
            {
                return m_config;
            }

            /** Called after the engine's GL context has been made current. */
            void setContextValid(bool valid)
            {
                m_markers.setContextValid(valid);
            }

            /** Selects the native window handle for captures (device stays wildcard). */
            void configureWindow(SDL_Window* window);

            /** Hotkey policy copied from EngineSettings into the module config. */
            bool isHotkeyCaptureEnabled() const
            {
                return m_config.captureOnHotkey;
            }

            std::string const & getCaptureHotkey() const
            {
                return m_config.captureHotkey;
            }

            /** Frame marker + auto capture, called before rendering starts. */
            void onFrameBegin();

            /** Closes the frame marker, called after presentation. */
            void onFrameEnd();

            /** One-shot marker for a named phase. */
            void markPhase(char const * name)
            {
                m_markers.setMarker(name);
            }

            void beginEvent(char const * name)
            {
                m_markers.beginEvent(name);
            }

            void endEvent()
            {
                m_markers.endEvent();
            }

            void setMarker(char const * name)
            {
                m_markers.setMarker(name);
            }

            void nameTexture(uint32_t name, char const * label);
            void nameFramebuffer(uint32_t name, char const * label);
            void nameBuffer(uint32_t name, char const * label);
            void nameShader(uint32_t name, char const * label);
            void nameProgram(uint32_t name, char const * label);
            void nameRenderbuffer(uint32_t name, char const * label);
            void nameVertexArray(uint32_t name, char const * label);

            bool triggerCapture()
            {
                return m_enabled && m_capture.triggerCapture();
            }

            bool startCapture()
            {
                return m_enabled && m_capture.startCapture();
            }

            bool endCapture()
            {
                return m_enabled && m_capture.endCapture();
            }

            bool isCaptureInProgress() const
            {
                return m_capture.isCaptureInProgress();
            }

            void setCapturePath(std::string const & path)
            {
                m_capture.setCapturePath(path);
            }

            std::string getLastCapturePath() const
            {
                return m_capture.getLastCapturePath();
            }

            /** Edge detector used by Engine's hotkey handling. */
            static bool isHotkeyEdge(bool down, bool wasDown)
            {
                return down && !wasDown;
            }

        private:
            RenderDocAPI m_api;
            RenderDocCapture m_capture;
            RenderDocMarkers m_markers;
            RenderDocConfig m_config{};
            bool m_enabled{false};
            bool m_autoCapturePending{false};
    };
} // namespace FIFE

#endif
