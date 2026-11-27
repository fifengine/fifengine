// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "vertexarray.h"

#include <bit>
#include <cstdint>

#include "video/glcore/glad/glcore_glad.h"

namespace FIFE
{
    GLCoreVertexArray::GLCoreVertexArray()
    {
#ifdef HAVE_OPENGL_CORE
        glGenVertexArrays(1, &m_name);
#endif
    }

    GLCoreVertexArray::~GLCoreVertexArray()
    {
#ifdef HAVE_OPENGL_CORE
        if (m_name != 0) {
            glDeleteVertexArrays(1, &m_name);
        }
#endif
    }

    void GLCoreVertexArray::bindAttribute(uint32_t location, int32_t componentCount, size_t stride, size_t offset) const
    {
#ifdef HAVE_OPENGL_CORE
        glBindVertexArray(m_name);
#endif
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(
            location,
            componentCount,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(stride),
            std::bit_cast<void const *>(static_cast<uintptr_t>(offset)));
    }

    void GLCoreVertexArray::bind() const
    {
#ifdef HAVE_OPENGL_CORE
        glBindVertexArray(m_name);
#endif
    }

    void GLCoreVertexArray::unbind()
    {
#ifdef HAVE_OPENGL_CORE
        glBindVertexArray(0);
#endif
    }
} // namespace FIFE
