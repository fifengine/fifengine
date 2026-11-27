// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "debug/renderdoc/renderdocmarkers.h"

#include "debug/diagnosticsphases.h"
#include "util/log/logger.h"

// KHR_debug values from the GL specification. Kept as constants here so the
// public header stays free of glad/GL types.
namespace
{
    constexpr uint32_t kDebugSourceApplication = 0x824A; // GL_DEBUG_SOURCE_APPLICATION
    constexpr uint32_t kDebugTypeMarker        = 0x8268; // GL_DEBUG_TYPE_MARKER
    constexpr uint32_t kDebugSeverityNotify    = 0x826B; // GL_DEBUG_SEVERITY_NOTIFICATION
    constexpr uint32_t kGLTexture              = 0x1702; // GL_TEXTURE (not GL_TEXTURE_2D 0x0DE1)
    constexpr uint32_t kGLRenderbuffer         = 0x8D41; // GL_RENDERBUFFER
    constexpr uint32_t kGLFramebuffer          = 0x8D40; // GL_FRAMEBUFFER
    constexpr uint32_t kGLBuffer               = 0x82E0; // GL_BUFFER
    constexpr uint32_t kGLShader               = 0x82E1; // GL_SHADER
    constexpr uint32_t kGLProgram              = 0x82E2; // GL_PROGRAM
    constexpr uint32_t kGLVertexArray          = 0x8074; // GL_VERTEX_ARRAY

    FIFE::Logger& _log()
    {
        static FIFE::Logger log(LM_VIDEO);
        return log;
    }
} // namespace

#if defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_ES2) || defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31)
    #include "video/glcore/glad/glcore_glad.h"
#endif

namespace FIFE
{
    char const * passName(RenderDocPass pass)
    {
        switch (pass) {
        case RenderDocPass::Frame:
            return kPhaseFrame;
        case RenderDocPass::Offscreen:
            return kPhaseOffscreen;
        case RenderDocPass::Target:
            return kPhaseTarget;
        case RenderDocPass::World:
            return kPhaseWorld;
        case RenderDocPass::Gui:
            return kPhaseGui;
        case RenderDocPass::Cursor:
            return kPhaseCursor;
        case RenderDocPass::Movie:
            return kPhaseMovie;
        case RenderDocPass::Shadow:
            return "Shadow Pass";
        case RenderDocPass::Geometry:
            return "Geometry Pass";
        case RenderDocPass::Lighting:
            return "Lighting Pass";
        case RenderDocPass::PostProcessing:
            return "Post Processing";
        case RenderDocPass::Bloom:
            return "Bloom";
        case RenderDocPass::Tonemapping:
            return "Tonemapping";
        case RenderDocPass::UI:
            return kPhaseGui;
        }
        return "Unknown Pass";
    }

    RenderDocMarkers::RenderDocMarkers(RenderDocAPI const & api) : m_api(api)
    {
#if defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_ES2) || defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31)
        if (GLAD_GL_KHR_debug != 0) {
            m_gl.pushDebugGroup     = glad_glPushDebugGroup;
            m_gl.popDebugGroup      = glad_glPopDebugGroup;
            m_gl.debugMessageInsert = glad_glDebugMessageInsert;
            m_gl.objectLabel        = glad_glObjectLabel;
        }
#endif
    }

    bool RenderDocMarkers::ready() const
    {
        return m_enabled && m_contextValid && m_api.isInitialized() && m_gl.pushDebugGroup != nullptr &&
               m_gl.popDebugGroup != nullptr;
    }

    void RenderDocMarkers::warnUnsupportedOnce()
    {
        if (m_warnedUnsupported) {
            return;
        }
        m_warnedUnsupported = true;
        FL_WARN(_log(), "RenderDoc markers unavailable: GL_KHR_debug is not supported by this context");
    }

    void RenderDocMarkers::beginEvent(char const * name)
    {
        if (name == nullptr || !ready()) {
            if (name != nullptr && m_enabled && m_contextValid && m_api.isInitialized()) {
                warnUnsupportedOnce();
            }
            return;
        }
        m_gl.pushDebugGroup(kDebugSourceApplication, 0, -1, name);
        ++m_depth;
    }

    void RenderDocMarkers::endEvent()
    {
        if (!ready() || m_depth == 0) {
            return;
        }
        m_gl.popDebugGroup();
        --m_depth;
    }

    void RenderDocMarkers::setMarker(char const * name)
    {
        if (name == nullptr || !m_enabled || !m_contextValid || !m_api.isInitialized()) {
            return;
        }
        if (m_gl.debugMessageInsert == nullptr) {
            warnUnsupportedOnce();
            return;
        }
        m_gl.debugMessageInsert(kDebugSourceApplication, kDebugTypeMarker, 0, kDebugSeverityNotify, -1, name);
    }

    void RenderDocMarkers::label(uint32_t identifier, uint32_t name, char const * labelText)
    {
        if (labelText == nullptr || !m_enabled || !m_contextValid || !m_api.isInitialized()) {
            return;
        }
        if (m_gl.objectLabel == nullptr) {
            warnUnsupportedOnce();
            return;
        }
        m_gl.objectLabel(identifier, name, -1, labelText);
    }

    void RenderDocMarkers::nameTexture(uint32_t name, char const * labelText)
    {
        label(kGLTexture, name, labelText);
    }

    void RenderDocMarkers::nameFramebuffer(uint32_t name, char const * labelText)
    {
        label(kGLFramebuffer, name, labelText);
    }

    void RenderDocMarkers::nameBuffer(uint32_t name, char const * labelText)
    {
        label(kGLBuffer, name, labelText);
    }

    void RenderDocMarkers::nameShader(uint32_t name, char const * labelText)
    {
        label(kGLShader, name, labelText);
    }

    void RenderDocMarkers::nameProgram(uint32_t name, char const * labelText)
    {
        label(kGLProgram, name, labelText);
    }

    void RenderDocMarkers::nameRenderbuffer(uint32_t name, char const * labelText)
    {
        label(kGLRenderbuffer, name, labelText);
    }

    void RenderDocMarkers::nameVertexArray(uint32_t name, char const * labelText)
    {
        label(kGLVertexArray, name, labelText);
    }
} // namespace FIFE
