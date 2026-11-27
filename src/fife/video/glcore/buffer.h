// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_BUFFER_H
#define FIFE_GLCORE_BUFFER_H

#include <cstddef>
#include <cstdint>

#include "platform.h"

namespace FIFE
{
    class FIFE_API GLCoreBuffer
    {
        public:
            enum class Kind : uint8_t
            {
                Array,
                Element
            };

            GLCoreBuffer(Kind kind, uint32_t usage);
            ~GLCoreBuffer();

            GLCoreBuffer(GLCoreBuffer const &)            = delete;
            GLCoreBuffer& operator=(GLCoreBuffer const &) = delete;

            void upload(void const * data, size_t byteCount) const;
            void bind() const;
            uint32_t name() const
            {
                return m_name;
            }

        private:
            uint32_t m_name{0};
            uint32_t m_target{0};
            uint32_t m_usage{0};
    };

    FIFE_API uint32_t glcoreQuadIndexBuffer();
} // namespace FIFE

#endif
