// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_SHADERLOADER_H
#define FIFE_GLCORE_SHADERLOADER_H

#include <array>
#include <cstdint>
#include <memory>

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
