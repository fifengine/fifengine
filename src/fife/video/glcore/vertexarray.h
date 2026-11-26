// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_VERTEXARRAY_H
#define FIFE_GLCORE_VERTEXARRAY_H

#include <cstddef>
#include <cstdint>

#include "platform.h"

namespace FIFE
{
    class FIFE_API GLCoreVertexArray
    {
        public:
            GLCoreVertexArray();
            ~GLCoreVertexArray();

            GLCoreVertexArray(GLCoreVertexArray const &)            = delete;
            GLCoreVertexArray& operator=(GLCoreVertexArray const &) = delete;

            void bindAttribute(uint32_t location, int32_t componentCount, size_t stride, size_t offset);
            void bind() const;
            static void unbind();
            uint32_t name() const
            {
                return m_name;
            }

        private:
            uint32_t m_name{0};
    };
} // namespace FIFE

#endif
