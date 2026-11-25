// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "shaderloader.h"

#include <memory>
#include <stdexcept>
#include <string>

#include "video/glcore/glad/glcore_glad.h"
#include "video/glcore/shader_compat.glsl.h"

namespace FIFE
{
    namespace
    {
        constexpr char const * spriteVS = R"(
GLCORE_ATTRIBUTE vec2 aPosition;
GLCORE_ATTRIBUTE vec2 aTexCoord;
GLCORE_ATTRIBUTE vec4 aColor;
GLCORE_VARYING_OUT vec2 vTexCoord;
GLCORE_VARYING_OUT vec4 vColor;
uniform mat4 uProjection;
void main() {
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    vTexCoord = aTexCoord;
    vColor = aColor;
}
)";

        constexpr char const * spriteFS = R"(
GLCORE_VARYING_IN vec2 vTexCoord;
GLCORE_VARYING_IN vec4 vColor;
uniform sampler2D uTexture0;
uniform vec4 uColor;
uniform float uAlphaTest;
void main() {
    vec4 c = GLCORE_TEXTURE(uTexture0, vTexCoord) * vColor * uColor;
    if (c.a < uAlphaTest) discard;
    GLCORE_FRAGCOLOR = c;
}
)";

        constexpr char const * primitiveVS = R"(
GLCORE_ATTRIBUTE vec2 aPosition;
GLCORE_ATTRIBUTE vec4 aColor;
GLCORE_VARYING_OUT vec4 vColor;
uniform mat4 uProjection;
void main() {
    gl_Position = uProjection * vec4(aPosition, 0.0, 1.0);
    vColor = aColor;
}
)";

        constexpr char const * primitiveFS = R"(
GLCORE_VARYING_IN vec4 vColor;
uniform vec4 uColor;
void main() {
    GLCORE_FRAGCOLOR = vColor * uColor;
}
)";

        constexpr char const * overlayFS = R"(
GLCORE_VARYING_IN vec2 vTexCoord;
GLCORE_VARYING_IN vec4 vColor;
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform vec4 uFactor;
uniform int uOverlayMode;
uniform vec4 uColor;
uniform float uAlphaTest;
void main() {
    vec4 base = GLCORE_TEXTURE(uTexture0, vTexCoord) * vColor;
    vec4 result = base;
    if (uOverlayMode == 1) {
        vec4 overlay = GLCORE_TEXTURE(uTexture1, vTexCoord);
        result.rgb = mix(base.rgb, uFactor.rgb, overlay.a * uFactor.a);
    } else if (uOverlayMode == 2) {
        vec4 overlay = GLCORE_TEXTURE(uTexture1, vTexCoord);
        result.rgb = mix(base.rgb, overlay.rgb, uFactor.a);
    } else if (uOverlayMode == 3) {
        vec4 overlay = GLCORE_TEXTURE(uTexture1, vTexCoord);
        result.rgb = base.rgb * overlay.rgb;
    }
    result *= uColor;
    if (result.a < uAlphaTest) discard;
    GLCORE_FRAGCOLOR = result;
}
)";

        constexpr char const * lightFS = R"(
uniform vec4 uColor;
uniform float uIntensity;
void main() {
    GLCORE_FRAGCOLOR = vec4(uColor.rgb * uIntensity, uColor.a);
}
)";

        struct ProgramSource
        {
                char const * vs;
                char const * fs;
        };

        ProgramSource sourceFor(GLCoreProgram program)
        {
            switch (program) {
            case GLCoreProgram::Sprite:
                return {spriteVS, spriteFS};
            case GLCoreProgram::Primitive:
                return {primitiveVS, primitiveFS};
            case GLCoreProgram::Overlay:
                return {spriteVS, overlayFS};
            case GLCoreProgram::Light:
                return {primitiveVS, lightFS};
            }
            return {spriteVS, spriteFS};
        }
    } // namespace

    ShaderDialect dialectFor(GLProfile profile)
    {
        switch (profile) {
        case GLProfile::Core33:
            return ShaderDialect::GLSL_330;
        case GLProfile::ES2:
            return ShaderDialect::ESSL_100;
        case GLProfile::Core43:
            return ShaderDialect::GLSL_430;
        case GLProfile::ES31:
            return ShaderDialect::ESSL_310;
        }
        return ShaderDialect::GLSL_330;
    }

    std::string const & preambleFor(ShaderDialect dialect)
    {
        // Keep the Tier A GLCORE_* aliases so the existing four program sources
        // compile unchanged in every dialect.
        constexpr char const * legacyGlsl = R"(
#define GLCORE_ATTRIBUTE in
#define GLCORE_VARYING_OUT out
#define GLCORE_VARYING_IN in
#define GLCORE_TEXTURE texture
out vec4 glcore_FragColor;
#define GLCORE_FRAGCOLOR glcore_FragColor
)";
        static std::string const glsl330  = std::string("#version 330 core\n") + kShaderCompatMacros + legacyGlsl;
        static std::string const glsl430  = std::string("#version 430 core\n") + kShaderCompatMacros + legacyGlsl;
        static std::string const essl100  = std::string(R"(#version 100
precision mediump float;
precision mediump int;
#define FIFE_VERTEX_IN attribute
#define FIFE_VERTEX_OUT varying
#define FIFE_FRAG_IN varying
#define FIFE_FRAG_OUT_DECL
#define FIFE_FRAG_COLOR gl_FragColor
#define FIFE_TEXTURE texture2D
)") + R"(
#define GLCORE_ATTRIBUTE attribute
#define GLCORE_VARYING_OUT varying
#define GLCORE_VARYING_IN varying
#define GLCORE_TEXTURE texture2D
#define GLCORE_FRAGCOLOR gl_FragColor
)";
        static std::string const essl310  = R"(#version 310 es
precision highp float;
precision highp int;
#define FIFE_VERTEX_IN in
#define FIFE_VERTEX_OUT out
#define FIFE_FRAG_IN in
#define FIFE_FRAG_OUT_DECL out vec4 fife_fragColor;
#define FIFE_FRAG_COLOR fife_fragColor
#define FIFE_TEXTURE texture
#define GLCORE_ATTRIBUTE in
#define GLCORE_VARYING_OUT out
#define GLCORE_VARYING_IN in
#define GLCORE_TEXTURE texture
out vec4 glcore_FragColor;
#define GLCORE_FRAGCOLOR glcore_FragColor
)";

        switch (dialect) {
        case ShaderDialect::GLSL_330:
            return glsl330;
        case ShaderDialect::GLSL_430:
            return glsl430;
        case ShaderDialect::ESSL_100:
            return essl100;
        case ShaderDialect::ESSL_310:
            return essl310;
        }
        return glsl330;
    }

    bool supportsProgram(ShaderDialect dialect, BuiltinProgram program)
    {
        bool const particle =
            program == BuiltinProgram::ParticleCompute || program == BuiltinProgram::ParticleInstanced;
        return !particle || dialect == ShaderDialect::GLSL_430 || dialect == ShaderDialect::ESSL_310;
    }

    namespace
    {
        constexpr char const * particleComputeSource = R"(
layout(local_size_x = 64) in;
layout(std430, binding = 0) buffer ParticleBuffer {
    vec4 positionLife[];
};
uniform float uDelta;
void main() {
    uint index = gl_GlobalInvocationID.x;
    vec4 value = positionLife[index];
    value.xy += value.zw * uDelta;
    positionLife[index] = value;
}
)";

        constexpr char const * particleInstancedVS = R"(
FIFE_VERTEX_IN vec2 aCorner;
uniform mat4 uViewProjection;
layout(std430, binding = 0) buffer ParticleBuffer {
    vec4 positionLife[];
};
out vec2 vUv;
void main() {
    vec4 value = positionLife[gl_InstanceID];
    vUv = aCorner * 0.5 + 0.5;
    gl_Position = uViewProjection * vec4(value.xy + aCorner * 0.01, 0.0, 1.0);
}
)";

        constexpr char const * particleInstancedFS = R"(
FIFE_FRAG_IN vec2 vUv;
uniform sampler2D uAtlas;
FIFE_FRAG_OUT_DECL
void main() {
    FIFE_FRAG_COLOR = texture(uAtlas, vUv);
}
)";
    } // namespace

    std::string const & builtinPreamble(ShaderDialect dialect)
    {
        static std::string const core = std::string("#version 430 core\n") + kShaderCompatMacros;
        static std::string const es =
            std::string("#version 310 es\nprecision highp float;\nprecision highp int;\n") + kShaderCompatMacros;
        return dialect == ShaderDialect::ESSL_310 ? es : core;
    }

    char const * builtinSource(BuiltinProgram program, bool vertex)
    {
        switch (program) {
        case BuiltinProgram::ParticleCompute:
            return particleComputeSource;
        case BuiltinProgram::ParticleInstanced:
            return vertex ? particleInstancedVS : particleInstancedFS;
        default:
            return nullptr;
        }
    }

    bool compileBuiltin(ShaderDialect dialect, BuiltinProgram program, std::string& error)
    {
        if (!supportsProgram(dialect, program)) {
            error = "Program is not supported in this dialect";
            return false;
        }
        if (program == BuiltinProgram::ParticleCompute) {
#if !defined(HAVE_OPENGL_43) && !defined(HAVE_OPENGLES_31)
            error = "Compute shaders require a Tier B profile";
            return false;
#else
            std::string const source = builtinPreamble(dialect) + particleComputeSource;
            uint32_t const shader    = glCreateShader(GL_COMPUTE_SHADER);
            char const * sourcePtr   = source.c_str();
            glShaderSource(shader, 1, &sourcePtr, nullptr);
            glCompileShader(shader);
            int status = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
            if (status == 0) {
                GLchar log[1024]{};
                glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
                error = log[0] != '\0' ? log : "Compute shader failed to compile";
                glDeleteShader(shader);
                return false;
            }
            uint32_t const programId = glCreateProgram();
            glAttachShader(programId, shader);
            glLinkProgram(programId);
            glGetProgramiv(programId, GL_LINK_STATUS, &status);
            glDeleteShader(shader);
            glDeleteProgram(programId);
            if (status == 0) {
                error = "Compute program failed to link";
                return false;
            }
            error.clear();
            return true;
#endif
        }
        if (program == BuiltinProgram::ParticleInstanced) {
#if !defined(HAVE_OPENGL_43) && !defined(HAVE_OPENGLES_31)
            error = "Instanced particles require a Tier B profile";
            return false;
#else
            GLCoreShader shader(
                dialect == ShaderDialect::ESSL_310 ? GLProfile::ES31 : GLProfile::Core43,
                (builtinPreamble(dialect) + particleInstancedVS).c_str(),
                (builtinPreamble(dialect) + particleInstancedFS).c_str());
            if (!shader.isLinked()) {
                error = "Instanced program failed to compile";
                return false;
            }
            return true;
#endif
        }
        error.clear();
        return true;
    }

    char const * glcorePreamble(GLProfile profile)
    {
        static std::string const core = preambleFor(ShaderDialect::GLSL_330);
        static std::string const es   = preambleFor(ShaderDialect::ESSL_100);
        return profile == GLProfile::Core33 ? core.c_str() : es.c_str();
    }

    ShaderLoader::ShaderLoader(GLProfile profile)
    {
        char const * const preamble = preambleFor(dialectFor(profile)).c_str();
        for (size_t i = 0U; i < m_programs.size(); ++i) {
            ProgramSource const source = sourceFor(static_cast<GLCoreProgram>(i));
            std::string const vs       = std::string(preamble) + source.vs;
            std::string const fs       = std::string(preamble) + source.fs;
            m_programs[i]              = std::make_unique<GLCoreShader>(profile, vs.c_str(), fs.c_str());
        }
    }

    ShaderLoader::~ShaderLoader() = default;

    GLCoreShader const & ShaderLoader::get(GLCoreProgram program) const
    {
        auto const & shader = m_programs[static_cast<size_t>(program)];
        if (!shader) {
            throw std::runtime_error("GLCore shader program is null");
        }
        return *shader;
    }

    int32_t ShaderLoader::uniform(GLCoreProgram program, char const * name) const
    {
        return get(program).uniform(name);
    }
} // namespace FIFE
