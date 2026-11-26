// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_STATE_H
#define FIFE_GLCORE_STATE_H

#include <array>
#include <cstdint>

#include "platform.h"
#include "util/structures/rect.h"

namespace FIFE
{
    class FIFE_API GLCoreState
    {
        public:
            GLCoreState();

            void reset();
            void useProgram(uint32_t program);
            void bindTexture(uint32_t unit, uint32_t texture);
            void setBlend(uint32_t src, uint32_t dst);
            void setDepthTest(bool enabled, uint32_t func);
            void setScissor(Rect const & area, bool enabled);
            void setStencil(bool enabled, uint8_t ref, uint32_t op, uint32_t func);
            void setScissorEnabled(bool enabled);
            void setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
            void setProjection(float left, float right, float top, float bottom, float nearZ, float farZ);

            float const * projection() const
            {
                return m_projection.data();
            }

        private:
            uint32_t m_program{0};
            std::array<uint32_t, 4> m_textures{};
            uint32_t m_activeUnit{0};
            uint32_t m_blendSrc{0};
            uint32_t m_blendDst{0};
            bool m_depthEnabled{false};
            uint32_t m_depthFunc{0};
            bool m_scissorEnabled{false};
            Rect m_scissor{};
            bool m_stencilEnabled{false};
            uint8_t m_stencilRef{0};
            uint32_t m_stencilOp{0};
            uint32_t m_stencilFunc{0};
            std::array<uint32_t, 4> m_viewport{};
            std::array<float, 16> m_projection{};
    };
} // namespace FIFE

#endif
