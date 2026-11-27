// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "shader.h"

#include <array>
#include <string>

#include "video/glcore/glad/glcore_glad.h"

namespace FIFE
{
    namespace
    {
        uint32_t compileShader(uint32_t type, char const * source)
        {
            uint32_t const shader = glCreateShader(type);
            glShaderSource(shader, 1, &source, nullptr);
            glCompileShader(shader);
            int status = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if (status == 0) {
                std::array<char, 2048> log{};
                glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        }
    } // namespace

    GLCoreShader::GLCoreShader(GLProfile /*unused*/, char const * vsSource, char const * fsSource)
    {
        uint32_t const vs = compileShader(GL_VERTEX_SHADER, vsSource);
        uint32_t const fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
        if (vs == 0 || fs == 0) {
            if (vs != 0) {
                glDeleteShader(vs);
            }
            if (fs != 0) {
                glDeleteShader(fs);
            }
            return;
        }

        m_program = glCreateProgram();
        glAttachShader(m_program, vs);
        glAttachShader(m_program, fs);
        glBindAttribLocation(m_program, 0, "aPosition");
        glBindAttribLocation(m_program, 1, "aColor");
        glBindAttribLocation(m_program, 2, "aTexCoord");
        glLinkProgram(m_program);
        glDetachShader(m_program, vs);
        glDetachShader(m_program, fs);
        glDeleteShader(vs);
        glDeleteShader(fs);

        int status = 0;
        glGetProgramiv(m_program, GL_LINK_STATUS, &status);
        m_linked = status != 0;
    }

    GLCoreShader::~GLCoreShader()
    {
        if (m_program != 0) {
            glDeleteProgram(m_program);
        }
    }

    int32_t GLCoreShader::uniform(char const * name) const
    {
        auto const it = m_uniforms.find(name);
        if (it != m_uniforms.end()) {
            return it->second;
        }
        int32_t const location = glGetUniformLocation(m_program, name);
        m_uniforms.emplace(name, location);
        return location;
    }

    void GLCoreShader::bind() const
    {
        if (m_program != 0) {
            glUseProgram(m_program);
        }
    }
} // namespace FIFE
