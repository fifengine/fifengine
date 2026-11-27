// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_RENDERDOC_MARKERS_H
#define FIFE_DEBUG_RENDERDOC_MARKERS_H

#include <cstdint>

#include "debug/renderdoc/renderdocapi.h"
#include "platform.h"

namespace FIFE
{
    /** Named regions used by FIFE and by games with additional passes.
     *
     * FIFE itself only emits Frame, Offscreen, Target, World, GUI, Cursor and
     * Movie markers: the engine has no shadow/geometry/lighting/bloom passes.
     * The remaining names are provided for hosts that own such passes.
     */
    enum class RenderDocPass : uint8_t
    {
        Frame,
        Offscreen,
        Target,
        World,
        Gui,
        Cursor,
        Movie,
        Shadow,
        Geometry,
        Lighting,
        PostProcessing,
        Bloom,
        Tonemapping,
        UI
    };

    FIFE_API char const * passName(RenderDocPass pass);

    /** GL_KHR_debug entry points, injectable so markers can be tested
     * without a current OpenGL context. */
    struct RenderDocGLFunctions
    {
            using PushDebugGroupFn     = void (*)(uint32_t, uint32_t, int32_t, char const *);
            using PopDebugGroupFn      = void (*)();
            using DebugMessageInsertFn = void (*)(uint32_t, uint32_t, uint32_t, uint32_t, int32_t, char const *);
            using ObjectLabelFn        = void (*)(uint32_t, uint32_t, int32_t, char const *);

            PushDebugGroupFn pushDebugGroup{nullptr};
            PopDebugGroupFn popDebugGroup{nullptr};
            DebugMessageInsertFn debugMessageInsert{nullptr};
            ObjectLabelFn objectLabel{nullptr};
    };

    class FIFE_API RenderDocMarkers
    {
        public:
            explicit RenderDocMarkers(RenderDocAPI const & api);

            /** Replaces the GL table (test seam). */
            void setGLFunctions(RenderDocGLFunctions const & functions)
            {
                m_gl = functions;
            }

            /** GL calls are only issued while a context is current on this thread. */
            void setContextValid(bool valid)
            {
                m_contextValid = valid;
            }

            void setEnabled(bool enabled)
            {
                m_enabled = enabled;
            }

            bool isEnabled() const
            {
                return m_enabled;
            }

            /** Drops any open region so re-initialisation cannot emit an
             * unmatched glPopDebugGroup(). */
            void reset()
            {
                m_depth = 0;
            }

            void beginEvent(char const * name);
            void endEvent();
            void setMarker(char const * name);

            void nameTexture(uint32_t name, char const * label);
            void nameFramebuffer(uint32_t name, char const * label);
            void nameBuffer(uint32_t name, char const * label);
            void nameShader(uint32_t name, char const * label);
            void nameProgram(uint32_t name, char const * label);
            void nameRenderbuffer(uint32_t name, char const * label);
            void nameVertexArray(uint32_t name, char const * label);

        private:
            RenderDocAPI const & m_api;
            RenderDocGLFunctions m_gl{};
            bool m_enabled{true};
            bool m_contextValid{false};
            bool m_warnedUnsupported{false};
            int m_depth{0};

            bool ready() const;
            void warnUnsupportedOnce();
            void label(uint32_t identifier, uint32_t name, char const * labelText);
    };
} // namespace FIFE

#endif
