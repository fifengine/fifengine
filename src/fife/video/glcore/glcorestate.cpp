// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "glcorestate.h"

#include "video/glcore/glad/glcore_glad.h"

namespace FIFE
{
    GLCoreState::GLCoreState()
    {
        reset();
    }

    void GLCoreState::reset()
    {
        m_program = 0;
        m_textures.fill(0);
        m_activeUnit     = 0;
        m_blendSrc       = 0;
        m_blendDst       = 0;
        m_depthEnabled   = false;
        m_depthFunc      = 0;
        m_scissorEnabled = false;
        m_stencilEnabled = false;
        m_stencilRef     = 0;
        m_stencilOp      = 0;
        m_stencilFunc    = 0;
        m_viewport.fill(0);
        for (size_t i = 0U; i < m_projection.size(); ++i) {
            m_projection[i] = (i % 5U == 0U) ? 1.0F : 0.0F;
        }
    }

    void GLCoreState::useProgram(uint32_t program)
    {
        if (m_program == program) {
            return;
        }
        m_program = program;
        glUseProgram(program);
    }

    void GLCoreState::bindTexture(uint32_t unit, uint32_t texture)
    {
        if (unit >= m_textures.size() || m_textures[unit] == texture) {
            return;
        }
        m_textures[unit] = texture;
        if (m_activeUnit != unit) {
            glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
            m_activeUnit = unit;
        }
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void GLCoreState::setBlend(uint32_t src, uint32_t dst)
    {
        if (m_blendSrc == src && m_blendDst == dst) {
            return;
        }
        m_blendSrc = src;
        m_blendDst = dst;
        glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
    }

    void GLCoreState::setDepthTest(bool enabled, uint32_t func)
    {
        if (m_depthEnabled != enabled) {
            m_depthEnabled = enabled;
            if (enabled) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
        }
        if (m_depthFunc != func) {
            m_depthFunc = func;
            glDepthFunc(static_cast<GLenum>(func));
        }
    }

    void GLCoreState::setScissor(Rect const & area, bool enabled)
    {
        if (m_scissorEnabled != enabled) {
            m_scissorEnabled = enabled;
            if (enabled) {
                glEnable(GL_SCISSOR_TEST);
            } else {
                glDisable(GL_SCISSOR_TEST);
            }
        }
        if (m_scissor != area) {
            m_scissor = area;
            glScissor(area.x, area.y, area.w, area.h);
        }
    }

    void GLCoreState::setStencil(bool enabled, uint8_t ref, uint32_t op, uint32_t func)
    {
        if (m_stencilEnabled != enabled) {
            m_stencilEnabled = enabled;
            if (enabled) {
                glEnable(GL_STENCIL_TEST);
            } else {
                glDisable(GL_STENCIL_TEST);
            }
        }
        if (m_stencilRef != ref || m_stencilOp != op || m_stencilFunc != func) {
            m_stencilRef  = ref;
            m_stencilOp   = op;
            m_stencilFunc = func;
            glStencilFunc(static_cast<GLenum>(func), ref, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, static_cast<GLenum>(op));
        }
    }

    void GLCoreState::setScissorEnabled(bool enabled)
    {
        if (m_scissorEnabled == enabled) {
            return;
        }
        m_scissorEnabled = enabled;
        if (enabled) {
            glEnable(GL_SCISSOR_TEST);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    void GLCoreState::setViewport(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
    {
        std::array<uint32_t, 4> const next{x, y, w, h};
        if (m_viewport == next) {
            return;
        }
        m_viewport = next;
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(w), static_cast<GLsizei>(h));
    }

    void GLCoreState::setProjection(float left, float right, float top, float bottom, float nearZ, float farZ)
    {
        float const width  = right - left;
        float const height = top - bottom;
        float const depth  = farZ - nearZ;
        m_projection.fill(0.0F);
        m_projection[0]  = width != 0.0F ? 2.0F / width : 0.0F;
        m_projection[5]  = height != 0.0F ? -2.0F / height : 0.0F;
        m_projection[10] = depth != 0.0F ? -2.0F / depth : 0.0F;
        m_projection[12] = width != 0.0F ? -(right + left) / width : 0.0F;
        m_projection[13] = height != 0.0F ? (top + bottom) / height : 0.0F;
        m_projection[14] = depth != 0.0F ? -(farZ + nearZ) / depth : 0.0F;
        m_projection[15] = 1.0F;
    }
} // namespace FIFE
