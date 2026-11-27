// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// glad-only translation unit: no renderbackend.h / GLEW header here.
#include "glcore_draw.h"

#include <array>
#include <bit>
#include <cstdint>
#include <string>
#include <vector>

#include "video/glcore/buffer.h"
#include "video/glcore/glad/glcore_glad.h"
#include "video/glcore/vertexarray.h"

namespace FIFE::glcore_draw
{
    namespace
    {
#ifdef HAVE_OPENGL_CORE
        uint32_t primitiveVao = 0;
        uint32_t imageVao     = 0;
#endif
        uint32_t primitiveVbo = 0;
        uint32_t imageVbo     = 0;
        uint32_t imageIbo     = 0;

        void ensurePrimitiveBuffers()
        {
            if (primitiveVbo == 0) {
#ifdef HAVE_OPENGL_CORE
                glGenVertexArrays(1, &primitiveVao);
                glBindVertexArray(primitiveVao);
#endif
                glGenBuffers(1, &primitiveVbo);
            }
        }

        void ensureImageBuffers()
        {
            if (imageVbo == 0) {
#ifdef HAVE_OPENGL_CORE
                glGenVertexArrays(1, &imageVao);
                glBindVertexArray(imageVao);
#endif
                glGenBuffers(1, &imageVbo);
                glGenBuffers(1, &imageIbo);
            }
        }

        void quadIndices(size_t quadCount, std::vector<uint16_t>& out)
        {
            out.clear();
            out.reserve(quadCount * 6);
            for (uint16_t q = 0; q < static_cast<uint16_t>(quadCount); ++q) {
                uint16_t const b = static_cast<uint16_t>(q * 4);
                out.push_back(b);
                out.push_back(static_cast<uint16_t>(b + 1));
                out.push_back(static_cast<uint16_t>(b + 2));
                out.push_back(b);
                out.push_back(static_cast<uint16_t>(b + 2));
                out.push_back(static_cast<uint16_t>(b + 3));
            }
        }
    } // namespace

    std::string renderDescription()
    {
        auto const str = [](GLenum name) -> std::string {
            GLubyte const * value = glGetString(name);
            // glGetString returns GLubyte*, but the result is a null-terminated char string.
            return value != nullptr ? reinterpret_cast<char const *>(value) :
                                      "unknown"; // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        };
        return "OpenGL vendor=" + str(GL_VENDOR) + " renderer=" + str(GL_RENDERER) + " version=" + str(GL_VERSION) +
               " GLSL=" + str(GL_SHADING_LANGUAGE_VERSION);
    }

    void reset()
    {
#ifdef HAVE_OPENGL_CORE
        primitiveVao = 0;
        imageVao     = 0;
#endif
        primitiveVbo = 0;
        imageVbo     = 0;
        imageIbo     = 0;
    }

    int lastGlError()
    {
        return static_cast<int>(glGetError());
    }

    void clear()
    {
        glClearColor(0.0F, 0.0F, 0.0F, 0.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void setScissor(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        glScissor(x, y, w, h);
    }

    void setViewport(int32_t x, int32_t y, int32_t w, int32_t h)
    {
        glViewport(x, y, w, h);
    }

    void enableBlend()
    {
        glEnable(GL_BLEND);
    }

    void clearStencil()
    {
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
    }

    uint32_t createFramebuffer(int32_t w, int32_t h, uint32_t& outTexture)
    {
        glGenTextures(1, &outTexture);
        glBindTexture(GL_TEXTURE_2D, outTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        uint32_t fbo = 0;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTexture, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fbo;
    }

    void bindFramebuffer(uint32_t fbo, int32_t w, int32_t h)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, w, h);
    }

    void deleteFramebuffer(uint32_t fbo, uint32_t texture)
    {
        if (fbo != 0) {
            glDeleteFramebuffers(1, &fbo);
        }
        if (texture != 0) {
            glDeleteTextures(1, &texture);
        }
    }

    void readRGBA(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* out)
    {
        glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, out);
    }

    void drawBatches(
        float const * primitiveData,
        size_t primitiveFloats,
        std::vector<ImageBatchView> const & images,
        uint32_t primitiveProgram,
        uint32_t spriteProgram,
        uint32_t overlayProgram,
        float const * projection)
    {
        if (primitiveFloats != 0) {
            ensurePrimitiveBuffers();
            glUseProgram(primitiveProgram);
            glUniformMatrix4fv(glGetUniformLocation(primitiveProgram, "uProjection"), 1, GL_FALSE, projection);
            std::array<float, 4> const white{1.0F, 1.0F, 1.0F, 1.0F};
            glUniform4fv(glGetUniformLocation(primitiveProgram, "uColor"), 1, white.data());
#ifdef HAVE_OPENGL_CORE
            glBindVertexArray(primitiveVao);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, primitiveVbo);
            glBufferData(
                GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(primitiveFloats * sizeof(float)),
                primitiveData,
                GL_STREAM_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
                1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), std::bit_cast<void const *>(uintptr_t{2 * sizeof(float)}));
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(primitiveFloats / 6));
#ifdef HAVE_OPENGL_CORE
            glBindVertexArray(0);
#endif
        }

        if (!images.empty()) {
            ensureImageBuffers();
            glUseProgram(spriteProgram);
            glUniformMatrix4fv(glGetUniformLocation(spriteProgram, "uProjection"), 1, GL_FALSE, projection);
            glUniform1i(glGetUniformLocation(spriteProgram, "uTexture0"), 0);
            std::array<float, 4> const white{1.0F, 1.0F, 1.0F, 1.0F};
            glUniform4fv(glGetUniformLocation(spriteProgram, "uColor"), 1, white.data());
            glUniform1f(glGetUniformLocation(spriteProgram, "uAlphaTest"), 0.0F);

#ifdef HAVE_OPENGL_CORE
            glBindVertexArray(imageVao);
#endif
            for (ImageBatchView const & batch : images) {
                if (batch.floats == 0) {
                    continue;
                }
                bool const useOverlay = batch.secondTexture != 0;
                glUseProgram(useOverlay ? overlayProgram : spriteProgram);
                glUniformMatrix4fv(
                    glGetUniformLocation(useOverlay ? overlayProgram : spriteProgram, "uProjection"),
                    1,
                    GL_FALSE,
                    projection);
                glUniform1i(glGetUniformLocation(useOverlay ? overlayProgram : spriteProgram, "uTexture0"), 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, batch.texture);
                if (useOverlay) {
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, batch.secondTexture);
                    glUniform1i(glGetUniformLocation(overlayProgram, "uTexture1"), 1);
                    glUniform1i(
                        glGetUniformLocation(overlayProgram, "uOverlayMode"), static_cast<GLint>(batch.overlayMode));
                    std::array<float, 4> const factor{1.0F, 1.0F, 1.0F, 1.0F};
                    glUniform4fv(glGetUniformLocation(overlayProgram, "uFactor"), 1, factor.data());
                }
                glBindBuffer(GL_ARRAY_BUFFER, imageVbo);
                glBufferData(
                    GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(batch.floats * sizeof(float)), batch.data, GL_STREAM_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(
                    1,
                    4,
                    GL_FLOAT,
                    GL_FALSE,
                    8 * sizeof(float),
                    std::bit_cast<void const *>(uintptr_t{4 * sizeof(float)}));
                glEnableVertexAttribArray(2);
                glVertexAttribPointer(
                    2,
                    2,
                    GL_FLOAT,
                    GL_FALSE,
                    8 * sizeof(float),
                    std::bit_cast<void const *>(uintptr_t{2 * sizeof(float)}));

                std::vector<uint16_t> indices;
                quadIndices(batch.floats / 32, indices);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imageIbo);
                glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    static_cast<GLsizeiptr>(indices.size() * sizeof(uint16_t)),
                    indices.data(),
                    GL_STREAM_DRAW);
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, nullptr);
            }
#ifdef HAVE_OPENGL_CORE
            glBindVertexArray(0);
#endif
        }
    }
} // namespace FIFE::glcore_draw
