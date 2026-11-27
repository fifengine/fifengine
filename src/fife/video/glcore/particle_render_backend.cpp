// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// cppcheck-suppress-file unusedStructMember
// glad-only translation unit: no GLEW / renderbackend.h here.
#include "particle_render_backend.h"

#include <algorithm>
#include <string>

#include "video/glcore/glad/glcore_glad.h"
#include "video/glcore/shader.h"
#include "video/glcore/shaderloader.h"

namespace FIFE
{
#if !defined(HAVE_OPENGL_43) && !defined(HAVE_OPENGLES_31)
    GLCoreParticleRenderBackend::GLCoreParticleRenderBackend(GLProfile /*unused*/)
    {
    }
    GLCoreParticleRenderBackend::~GLCoreParticleRenderBackend() = default;
    uint32_t GLCoreParticleRenderBackend::clampCapacity(uint32_t requested) const
    {
        return std::min<uint32_t>(requested, 2048U);
    }
    void GLCoreParticleRenderBackend::setEmitterCapacity(
        [[maybe_unused]] uint32_t emitterId, [[maybe_unused]] uint32_t capacity)
    {
    }
    void GLCoreParticleRenderBackend::uploadNewParticles(
        [[maybe_unused]] uint32_t emitterId,
        [[maybe_unused]] ParticleGpuRecord const * records,
        [[maybe_unused]] uint32_t count,
        [[maybe_unused]] uint32_t cursor)
    {
    }
    void GLCoreParticleRenderBackend::releaseEmitter([[maybe_unused]] uint32_t emitterId)
    {
    }
    void GLCoreParticleRenderBackend::dispatch(
        [[maybe_unused]] uint32_t emitterId,
        [[maybe_unused]] uint32_t aliveCount,
        [[maybe_unused]] float dt,
        [[maybe_unused]] uint64_t simulationStep)
    {
    }
    void GLCoreParticleRenderBackend::draw(
        [[maybe_unused]] uint32_t emitterId,
        [[maybe_unused]] uint32_t aliveCount,
        [[maybe_unused]] Image const & atlas,
        [[maybe_unused]] float const * viewProjection,
        [[maybe_unused]] Rect const & rect)
    {
    }
    void GLCoreParticleRenderBackend::onContextRecreated()
    {
    }
#else
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
                glDeleteShader(shader);
                return 0;
            }
            return shader;
        }

        uint32_t linkProgram(char const * vs, char const * fs)
        {
            uint32_t const v = compileShader(GL_VERTEX_SHADER, vs);
            uint32_t const f = compileShader(GL_FRAGMENT_SHADER, fs);
            if (v == 0 || f == 0) {
                if (v != 0)
                    glDeleteShader(v);
                if (f != 0)
                    glDeleteShader(f);
                return 0;
            }
            uint32_t const p = glCreateProgram();
            glAttachShader(p, v);
            glAttachShader(p, f);
            glLinkProgram(p);
            glDeleteShader(v);
            glDeleteShader(f);
            int status = 0;
            glGetProgramiv(p, GL_LINK_STATUS, &status);
            if (status == 0) {
                glDeleteProgram(p);
                return 0;
            }
            return p;
        }

        struct GpuQuad
        {
                uint32_t vao{0};
                uint32_t vbo{0};
        };
    } // namespace

    GLCoreParticleRenderBackend::GLCoreParticleRenderBackend(GLProfile profile) : m_profile(profile)
    {
        // cppcheck-suppress virtualCallInConstructor
        onContextRecreated();
    }

    GLCoreParticleRenderBackend::~GLCoreParticleRenderBackend()
    {
        for (auto& entry : m_buffers) {
            if (entry.second.buffer != 0) {
                glDeleteBuffers(1, &entry.second.buffer);
            }
        }
        if (m_computeProgram != 0)
            glDeleteProgram(m_computeProgram);
        if (m_drawProgram != 0)
            glDeleteProgram(m_drawProgram);
        if (m_quad.vao != 0)
            glDeleteVertexArrays(1, &m_quad.vao);
        if (m_quad.vbo != 0)
            glDeleteBuffers(1, &m_quad.vbo);
    }

    uint32_t GLCoreParticleRenderBackend::clampCapacity(uint32_t requested) const
    {
        constexpr uint32_t hardCap = 262144U;
        uint32_t cap               = std::min(requested, hardCap);
        GLint maxSize              = 0;
        glGetIntegerv(GL_MAX_SHADER_STORAGE_BLOCK_SIZE, &maxSize);
        if (maxSize > 0) {
            uint64_t const count = static_cast<uint64_t>(maxSize) / (4U * sizeof(float));
            cap                  = std::min<uint32_t>(cap, static_cast<uint32_t>(std::min<uint64_t>(count, hardCap)));
        }
        return cap;
    }

    void GLCoreParticleRenderBackend::setEmitterCapacity(uint32_t emitterId, uint32_t capacity)
    {
        EmitterBuffer& buffer = m_buffers[emitterId];
        if (buffer.buffer != 0) {
            glDeleteBuffers(1, &buffer.buffer);
            buffer.buffer = 0;
        }
        uint32_t const cap = clampCapacity(capacity);
        glGenBuffers(1, &buffer.buffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer.buffer);
        glBufferData(
            GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(cap * 4U * sizeof(float)), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        buffer.capacity = cap;
        buffer.cursor   = 0;
    }

    void GLCoreParticleRenderBackend::uploadNewParticles(
        uint32_t emitterId, ParticleGpuRecord const * records, uint32_t count, uint32_t slot)
    {
        auto it = m_buffers.find(emitterId);
        if (it == m_buffers.end() || it->second.buffer == 0 || records == nullptr || count == 0 ||
            it->second.capacity == 0) {
            return;
        }
        // Pack the record into the vec4 layout expected by the shaders.
        float data[4]        = {records->position[0], records->position[1], records->velocity[0], records->velocity[1]};
        uint32_t const index = slot % it->second.capacity;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, it->second.buffer);
        glBufferSubData(
            GL_SHADER_STORAGE_BUFFER,
            static_cast<GLintptr>(index * 4U * sizeof(float)),
            static_cast<GLsizeiptr>(4U * sizeof(float)),
            data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        it->second.cursor = (index + count) % it->second.capacity;
    }

    void GLCoreParticleRenderBackend::releaseEmitter(uint32_t emitterId)
    {
        auto it = m_buffers.find(emitterId);
        if (it != m_buffers.end() && it->second.buffer != 0) {
            glDeleteBuffers(1, &it->second.buffer);
        }
        m_buffers.erase(emitterId);
    }

    void GLCoreParticleRenderBackend::dispatch(
        uint32_t emitterId, uint32_t aliveCount, float dt, uint64_t simulationStep)
    {
        if (m_lastDispatchedStep[emitterId] == simulationStep) {
            return;
        }
        m_lastDispatchedStep[emitterId] = simulationStep;
        auto it                         = m_buffers.find(emitterId);
        if (m_computeProgram == 0 || it == m_buffers.end() || it->second.buffer == 0 || aliveCount == 0) {
            return;
        }
        glUseProgram(m_computeProgram);
        glUniform1f(glGetUniformLocation(m_computeProgram, "uDelta"), dt);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it->second.buffer);
        glDispatchCompute((aliveCount + 63U) / 64U, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void GLCoreParticleRenderBackend::draw(
        uint32_t emitterId, uint32_t aliveCount, Image const &, float const * viewProjection, Rect const &)
    {
        auto it = m_buffers.find(emitterId);
        if (m_drawProgram == 0 || it == m_buffers.end() || it->second.buffer == 0 || aliveCount == 0) {
            return;
        }
        glUseProgram(m_drawProgram);
        glUniformMatrix4fv(glGetUniformLocation(m_drawProgram, "uViewProjection"), 1, GL_FALSE, viewProjection);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, it->second.buffer);
        glBindVertexArray(m_quad.vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(aliveCount));
        glBindVertexArray(0);
    }

    void GLCoreParticleRenderBackend::onContextRecreated()
    {
        for (auto& entry : m_buffers) {
            if (entry.second.buffer != 0) {
                glDeleteBuffers(1, &entry.second.buffer);
                entry.second.buffer = 0;
            }
            entry.second.cursor = 0;
        }
        m_lastDispatchedStep.clear();
        if (m_computeProgram != 0)
            glDeleteProgram(m_computeProgram);
        if (m_drawProgram != 0)
            glDeleteProgram(m_drawProgram);
        if (m_quad.vao != 0)
            glDeleteVertexArrays(1, &m_quad.vao);
        if (m_quad.vbo != 0)
            glDeleteBuffers(1, &m_quad.vbo);

        ShaderDialect const dialect  = dialectFor(m_profile);
        std::string const preamble   = builtinPreamble(dialect);
        char const * computeSource   = builtinSource(BuiltinProgram::ParticleCompute, true);
        uint32_t const computeShader = compileShader(GL_COMPUTE_SHADER, (preamble + computeSource).c_str());
        m_computeProgram             = glCreateProgram();
        glAttachShader(m_computeProgram, computeShader);
        glLinkProgram(m_computeProgram);
        glDeleteShader(computeShader);

        m_drawProgram = linkProgram(
            (preamble + builtinSource(BuiltinProgram::ParticleInstanced, true)).c_str(),
            (preamble + builtinSource(BuiltinProgram::ParticleInstanced, false)).c_str());

        float const corners[12] = {-1.0F, -1.0F, 1.0F, -1.0F, 1.0F, 1.0F, -1.0F, -1.0F, 1.0F, 1.0F, -1.0F, 1.0F};
        glGenVertexArrays(1, &m_quad.vao);
        glGenBuffers(1, &m_quad.vbo);
        glBindVertexArray(m_quad.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_quad.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(corners), corners, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glBindVertexArray(0);
    }
#endif
} // namespace FIFE
