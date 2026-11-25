// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_DRAW_H
#define FIFE_GLCORE_DRAW_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "platform.h"

namespace FIFE
{
    namespace glcore_draw
    {
        struct ImageBatchView
        {
                uint32_t texture{0};
                uint32_t secondTexture{0};
                uint32_t overlayMode{0};
                float const * data{nullptr};
                size_t floats{0};
        };

        /** GL_VENDOR / GL_RENDERER / GL_VERSION / GLSL version as one string.
         * Implemented here so no GL call leaks into the shell TU.
         */
        std::string renderDescription();

        /** Forget cached GL object names after a context is recreated. */
        void reset();

        void clear();
        void setScissor(int32_t x, int32_t y, int32_t w, int32_t h);
        void setViewport(int32_t x, int32_t y, int32_t w, int32_t h);
        void enableBlend();
        void clearStencil();

        uint32_t createFramebuffer(int32_t w, int32_t h, uint32_t& outTexture);
        void bindFramebuffer(uint32_t fbo, int32_t w, int32_t h);
        void deleteFramebuffer(uint32_t fbo, uint32_t texture);
        void readRGBA(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t* out);

        /** Flushes primitive and textured-quad batches. All GL entry points
         * used here come from the glad loader chosen for the active profile.
         */
        void drawBatches(
            float const * primitiveData,
            size_t primitiveFloats,
            std::vector<ImageBatchView> const & images,
            uint32_t primitiveProgram,
            uint32_t spriteProgram,
            uint32_t overlayProgram,
            float const * projection);
    } // namespace glcore_draw
} // namespace FIFE

#endif
