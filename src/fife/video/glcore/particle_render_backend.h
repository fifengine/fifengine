// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_PARTICLE_RENDER_BACKEND_H
#define FIFE_GLCORE_PARTICLE_RENDER_BACKEND_H

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "platform.h"
#include "video/glcore/glcontext.h"
#include "video/iparticlerenderbackend.h"

namespace FIFE
{
    class FIFE_API GLCoreParticleRenderBackend final : public IParticleRenderBackend
    {
        public:
            explicit GLCoreParticleRenderBackend(GLProfile profile);
            ~GLCoreParticleRenderBackend() override;

            uint32_t clampCapacity(uint32_t requested) const override;
            void setEmitterCapacity(uint32_t emitterId, uint32_t capacity) override;
            void uploadNewParticles(
                uint32_t emitterId, ParticleGpuRecord const * records, uint32_t count, uint32_t cursor) override;
            void releaseEmitter(uint32_t emitterId) override;
            void dispatch(uint32_t emitterId, uint32_t aliveCount, float dt, uint64_t simulationStep) override;
            void draw(
                uint32_t emitterId,
                uint32_t aliveCount,
                Image const & atlas,
                float const * viewProjection,
                Rect const & rect) override;
            void onContextRecreated() override; // cppcheck-suppress virtualCallInConstructor

            uint64_t lastDispatchStep(uint32_t emitterId) const
            {
#if defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31)
                auto const it = m_lastDispatchedStep.find(emitterId);
                return it == m_lastDispatchedStep.end() ? 0U : it->second;
#else
                static_cast<void>(emitterId);
                return 0U;
#endif
            }

        private:
            struct EmitterBuffer
            {
                    uint32_t buffer{0};
                    uint32_t capacity{0};
                    uint32_t cursor{0};
            };
            struct GpuQuad
            {
                    uint32_t vao{0};
                    uint32_t vbo{0};
            };

#if defined(HAVE_OPENGL_43) || defined(HAVE_OPENGLES_31)
            GLProfile m_profile{GLProfile::Core43};
            std::unordered_map<uint32_t, EmitterBuffer> m_buffers;
            std::unordered_map<uint32_t, uint64_t> m_lastDispatchedStep;
            GpuQuad m_quad;
            uint32_t m_computeProgram{0};
            uint32_t m_drawProgram{0};
#endif
    };
} // namespace FIFE

#endif
