// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include "glcore_test_env.h"
#include "util/base/exception.h"
#include "video/glcore/buffer.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/shaderloader.h"
#include "video/glcore/vertexarray.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

#ifdef HAVE_OPENGL_CORE
    #include "video/glcore/glad/core/glad.h"
#else
    #include "video/glcore/glad/es2/glad.h"
#endif

TEST_CASE("Indexed quad renders with the shared index buffer in core profile", "[glcore][buffer]")
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SKIP("SDL video unavailable");
    }
    FIFE::Window window;
    try {
        window.create({.width = 64, .height = 64, .opengl = true, .windowMode = FIFE::WindowMode::Windowed});
    } catch (FIFE::SDLException const &) {
        SKIP("OpenGL not available in this environment");
    }
    FIFE::GLContext ctx(FIFE::makeCore33Config());
    ctx.applyAttributes();
    if (!ctx.create(window.getSDLWindow())) {
        SKIP("OpenGL context not available in this environment");
    }

    glViewport(0, 0, 64, 64);
    FIFE::ShaderLoader loader(FIFE::GLProfile::Core33);
    FIFE::GLCoreShader const & shader = loader.get(FIFE::GLCoreProgram::Sprite);
    shader.bind();

    float const projection[16] = {
        1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F};
    glUniformMatrix4fv(shader.uniform("uProjection"), 1, GL_FALSE, projection);
    float const white[4] = {1.0F, 1.0F, 1.0F, 1.0F};
    glUniform4fv(shader.uniform("uColor"), 1, white);
    glUniform1f(shader.uniform("uAlphaTest"), 0.0F);
    glUniform1i(shader.uniform("uTexture0"), 0);

    unsigned texture = 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    unsigned char const pixels[16] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // x, y, u, v, r, g, b, a
    float const vertices[] = {-0.5F, -0.5F, 0.0F,  0.0F, 1.0F, 1.0F, 1.0F, 1.0F, 0.5F, -0.5F, 1.0F,
                              0.0F,  1.0F,  1.0F,  1.0F, 1.0F, 0.5F, 0.5F, 1.0F, 1.0F, 1.0F,  1.0F,
                              1.0F,  1.0F,  -0.5F, 0.5F, 0.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F};
    FIFE::GLCoreBuffer vbo(FIFE::GLCoreBuffer::Kind::Array, GL_STATIC_DRAW);
    vbo.upload(vertices, sizeof(vertices));
    vbo.bind();

    FIFE::GLCoreVertexArray vao;
    vao.bindAttribute(0, 2, 8 * sizeof(float), 0);
    vao.bindAttribute(
        static_cast<uint32_t>(glGetAttribLocation(shader.program(), "aTexCoord")),
        2,
        8 * sizeof(float),
        2 * sizeof(float));
    vao.bindAttribute(
        static_cast<uint32_t>(glGetAttribLocation(shader.program(), "aColor")),
        4,
        8 * sizeof(float),
        4 * sizeof(float));
    vao.bind();

    unsigned const ibo = FIFE::glcoreQuadIndexBuffer();
    REQUIRE(ibo != 0U);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    unsigned char centre[4]{};
    glReadPixels(32, 32, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, centre);
    CHECK(centre[0] == 255);
    CHECK(centre[3] == 255);

    glDeleteTextures(1, &texture);
    FIFE::GLCoreVertexArray::unbind();
}
