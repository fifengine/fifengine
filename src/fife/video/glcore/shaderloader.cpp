// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "shaderloader.h"

#include <memory>
#include <stdexcept>
#include <string>

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

    char const * glcorePreamble(GLProfile profile)
    {
        if (profile == GLProfile::Core33) {
            return R"(#version 330 core
#define GLCORE_ATTRIBUTE in
#define GLCORE_VARYING_OUT out
#define GLCORE_VARYING_IN in
#define GLCORE_TEXTURE texture
out vec4 glcore_FragColor;
#define GLCORE_FRAGCOLOR glcore_FragColor
)";
        }
        return R"(#version 100
precision mediump float;
precision mediump int;
#define GLCORE_ATTRIBUTE attribute
#define GLCORE_VARYING_OUT varying
#define GLCORE_VARYING_IN varying
#define GLCORE_TEXTURE texture2D
#define GLCORE_FRAGCOLOR gl_FragColor
)";
    }

    ShaderLoader::ShaderLoader(GLProfile profile)
    {
        char const * const preamble = glcorePreamble(profile);
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
