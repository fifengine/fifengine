// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "buffer.h"

#include "video/glcore/glad/glcore_glad.h"

namespace FIFE
{
    GLCoreBuffer::GLCoreBuffer(Kind kind, uint32_t usage) : m_usage(usage)
    {
        m_target = kind == Kind::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
        glGenBuffers(1, &m_name);
    }

    GLCoreBuffer::~GLCoreBuffer()
    {
        if (m_name != 0) {
            glDeleteBuffers(1, &m_name);
        }
    }

    void GLCoreBuffer::upload(void const * data, size_t byteCount)
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
        static uint32_t ibo = []() {
            uint32_t name = 0;
            glGenBuffers(1, &name);
            unsigned int const indices[6] = {0, 1, 2, 0, 2, 3};
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, name);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            return name;
        }();
        return ibo;
    }
} // namespace FIFE
