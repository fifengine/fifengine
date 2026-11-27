// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "buffer.h"

#include <array>

#include "video/glcore/glad/glcore_glad.h"

namespace FIFE
{
    GLCoreBuffer::GLCoreBuffer(Kind kind, uint32_t usage) :
        m_target(kind == Kind::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER), m_usage(usage)
    {
        glGenBuffers(1, &m_name);
    }

    GLCoreBuffer::~GLCoreBuffer()
    {
        if (m_name != 0) {
            glDeleteBuffers(1, &m_name);
        }
    }

    void GLCoreBuffer::upload(void const * data, size_t byteCount) const
    {
        glBindBuffer(m_target, m_name);
        glBufferData(m_target, static_cast<GLsizeiptr>(byteCount), data, static_cast<GLenum>(m_usage));
    }

    void GLCoreBuffer::bind() const
    {
        glBindBuffer(m_target, m_name);
    }

    uint32_t glcoreQuadIndexBuffer()
    {
        static uint32_t const ibo = []() {
            uint32_t name = 0;
            glGenBuffers(1, &name);
            std::array<unsigned int, 6> const indices{0U, 1U, 2U, 0U, 2U, 3U};
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, name);
            glBufferData(
                GL_ELEMENT_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                indices.data(),
                GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return name;
        }();
        return ibo;
    }
} // namespace FIFE
