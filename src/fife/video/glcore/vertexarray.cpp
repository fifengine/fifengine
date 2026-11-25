// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "vertexarray.h"

#ifdef HAVE_OPENGL_CORE
    #include "video/glcore/glad/core/glad.h"
#else
    #include "video/glcore/glad/es2/glad.h"
#endif

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

    void GLCoreVertexArray::bindAttribute(uint32_t location, int32_t componentCount, size_t stride, size_t offset)
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
            reinterpret_cast<void const *>(offset));
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
