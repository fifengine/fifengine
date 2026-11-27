// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_IPARTICLERENDERBACKEND_H
#define FIFE_IPARTICLERENDERBACKEND_H

#include <cstdint>

#include "platform.h"
#include "util/structures/rect.h"

namespace FIFE
{
    class Image;

    struct ParticleGpuRecord
    {
            float position[2];
            float velocity[2];
            float color[4];
            float size;
            float age;
            float life;
            uint32_t atlasFrame;
    };

    /** GL-free bridge for GPU-side particle integration.
     * Implemented by backends that support compute shaders; all other
     * backends leave RenderBackend::getParticleRenderBackend() null.
     */
    class FIFE_API IParticleRenderBackend;

    /** Returns the active backend's bridge, or null. Implemented in video/ so
     * model code never needs to include renderbackend.h (and GLEW).
     */
    FIFE_API IParticleRenderBackend* activeParticleRenderBackend();

    class FIFE_API IParticleRenderBackend
    {
        public:
            virtual ~IParticleRenderBackend() = default;

            virtual uint32_t clampCapacity(uint32_t requested) const               = 0;
            virtual void setEmitterCapacity(uint32_t emitterId, uint32_t capacity) = 0;
            virtual void uploadNewParticles(
                uint32_t emitterId, ParticleGpuRecord const * records, uint32_t count, uint32_t cursor)       = 0;
            virtual void releaseEmitter(uint32_t emitterId)                                                   = 0;
            virtual void dispatch(uint32_t emitterId, uint32_t aliveCount, float dt, uint64_t simulationStep) = 0;
            virtual void draw(
                uint32_t emitterId,
                uint32_t aliveCount,
                Image const & atlas,
                float const * viewProjection,
                Rect const & rect)            = 0;
            virtual void onContextRecreated() = 0;
    };
} // namespace FIFE

#endif
