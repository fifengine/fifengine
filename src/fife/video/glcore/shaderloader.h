// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_SHADERLOADER_H
#define FIFE_GLCORE_SHADERLOADER_H

#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "platform.h"
#include "video/glcore/glcontext.h"
#include "video/glcore/shader.h"

namespace FIFE
{
    enum class GLCoreProgram : uint8_t
    {
        Sprite,
        Primitive,
        Overlay,
        Light
    };

    FIFE_API char const * glcorePreamble(GLProfile profile);

    enum class ShaderDialect : uint8_t
    {
        GLSL_330,
        GLSL_430,
        ESSL_100,
        ESSL_310
    };

    enum class BuiltinProgram : uint8_t
    {
        Sprite,
        Primitive,
        Overlay,
        Light,
        ParticleCompute,
        ParticleInstanced
    };

    FIFE_API ShaderDialect dialectFor(GLProfile profile);
    FIFE_API std::string const & preambleFor(ShaderDialect dialect);
    FIFE_API bool supportsProgram(ShaderDialect dialect, BuiltinProgram program);
    FIFE_API bool compileBuiltin(ShaderDialect dialect, BuiltinProgram program, std::string& error);
    FIFE_API char const * builtinSource(BuiltinProgram program, bool vertex);
    FIFE_API std::string const & builtinPreamble(ShaderDialect dialect);

    class FIFE_API ShaderLoader
    {
        public:
            explicit ShaderLoader(GLProfile profile);
            ~ShaderLoader();

            ShaderLoader(ShaderLoader const &)            = delete;
            ShaderLoader& operator=(ShaderLoader const &) = delete;

            GLCoreShader const & get(GLCoreProgram program) const;
            int32_t uniform(GLCoreProgram program, char const * name) const;

        private:
            std::array<std::unique_ptr<GLCoreShader>, 4> m_programs;
    };
} // namespace FIFE

#endif
