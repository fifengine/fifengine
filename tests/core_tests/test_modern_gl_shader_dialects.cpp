// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include <SDL3/SDL.h>

#include <string>

#include "util/base/exception.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/shaderloader.h"
#include "video/window/window.h"
#include <catch2/catch_test_macros.hpp> // NOLINT(build/include_alpha)

TEST_CASE("Built-in programs obey the four-dialect matrix", "[modern-gl][shaders]")
{
    CHECK(FIFE::preambleFor(FIFE::ShaderDialect::GLSL_330).starts_with("#version 330 core"));
    CHECK(FIFE::preambleFor(FIFE::ShaderDialect::GLSL_430).starts_with("#version 430 core"));
    CHECK(FIFE::preambleFor(FIFE::ShaderDialect::ESSL_100).starts_with("#version 100"));
    CHECK(FIFE::preambleFor(FIFE::ShaderDialect::ESSL_310).starts_with("#version 310 es"));
    CHECK(FIFE::preambleFor(FIFE::ShaderDialect::ESSL_310).find("precision ") != std::string::npos);

    constexpr FIFE::ShaderDialect all[] = {
        FIFE::ShaderDialect::GLSL_330,
        FIFE::ShaderDialect::GLSL_430,
        FIFE::ShaderDialect::ESSL_100,
        FIFE::ShaderDialect::ESSL_310};
    constexpr FIFE::BuiltinProgram old[] = {
        FIFE::BuiltinProgram::Sprite,
        FIFE::BuiltinProgram::Primitive,
        FIFE::BuiltinProgram::Overlay,
        FIFE::BuiltinProgram::Light};
    for (auto d : all) {
        for (auto p : old) {
            CHECK(FIFE::supportsProgram(d, p));
        }
    }
    for (auto p : {FIFE::BuiltinProgram::ParticleCompute, FIFE::BuiltinProgram::ParticleInstanced}) {
        CHECK_FALSE(FIFE::supportsProgram(FIFE::ShaderDialect::GLSL_330, p));
        CHECK_FALSE(FIFE::supportsProgram(FIFE::ShaderDialect::ESSL_100, p));
        CHECK(FIFE::supportsProgram(FIFE::ShaderDialect::GLSL_430, p));
        CHECK(FIFE::supportsProgram(FIFE::ShaderDialect::ESSL_310, p));
    }
}

TEST_CASE("Applicable programs compile in a real context", "[modern-gl][shaders][opengl]")
{
#if defined(HAVE_OPENGL_CORE) || defined(HAVE_OPENGL_43)
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
        SKIP("OpenGL not available in this environment");
    }
    std::string error;
    for (auto d : {FIFE::ShaderDialect::GLSL_330, FIFE::ShaderDialect::GLSL_430}) {
        for (auto p :
             {FIFE::BuiltinProgram::Sprite,
              FIFE::BuiltinProgram::Primitive,
              FIFE::BuiltinProgram::Overlay,
              FIFE::BuiltinProgram::Light}) {
            CHECK(FIFE::compileBuiltin(d, p, error));
        }
        if (d == FIFE::ShaderDialect::GLSL_430) {
            CHECK(FIFE::compileBuiltin(d, FIFE::BuiltinProgram::ParticleCompute, error));
            INFO("compute error: " << error);
            CHECK(FIFE::compileBuiltin(d, FIFE::BuiltinProgram::ParticleInstanced, error));
            INFO("instanced error: " << error);
        }
    }
#else
    SKIP("ES builds have no desktop GL context in this test");
#endif
}
