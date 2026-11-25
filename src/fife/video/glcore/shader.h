// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_SHADER_H
#define FIFE_GLCORE_SHADER_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "platform.h"
#include "video/glcore/glcontext.h"

namespace FIFE
{
    class GLCoreShader
    {
        public:
            GLCoreShader(GLProfile profile, char const * vsSource, char const * fsSource);
            ~GLCoreShader();

            GLCoreShader(GLCoreShader const &)            = delete;
            GLCoreShader& operator=(GLCoreShader const &) = delete;

            uint32_t program() const
            {
                return m_program;
            }

            bool isLinked() const
            {
                return m_linked;
            }

            int32_t uniform(char const * name) const;
            void bind() const;

        private:
            uint32_t m_program{0};
            bool m_linked{false};
            mutable std::unordered_map<std::string, int32_t> m_uniforms;
    };
} // namespace FIFE

#endif
