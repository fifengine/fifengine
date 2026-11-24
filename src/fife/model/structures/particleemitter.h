// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PARTICLEEMITTER_H
#define FIFE_PARTICLEEMITTER_H

#include "platform.h"

#include "model/metamodel/modelcoords.h"
#include "model/structures/particleconfig.h"

#include <cstddef>
#include <cstdint>
#include <random>
#include <vector>

namespace FIFE
{
    class ParticleEmitter
    {
        public:
            explicit ParticleEmitter(ParticleConfig const & config);
            virtual ~ParticleEmitter() = default;

            ParticleEmitter(ParticleEmitter const &)            = delete;
            ParticleEmitter & operator=(ParticleEmitter const &) = delete;

            void setSeed(uint32_t seed);
            virtual void update(float dt);

            size_t getAliveCount() const
            {
                return m_alive;
            }

            size_t getEmittedCount() const
            {
                return m_emitted;
            }

            uint32_t getCapacity() const
            {
                return m_capacity;
            }

            uint32_t getMaxParticles() const;
            void setMaxParticles(uint32_t maxParticles);

            ParticleSnapshot getParticle(size_t slot) const;
            ExactModelCoordinate getParticleWorldPosition(size_t slot) const;

        private:
            uint32_t effectiveCapacity() const;
            void rebuildPool();
            bool spawn();
            void refreshVisuals(ParticleSnapshot & particle) const;

            ParticleConfig m_config;
            uint32_t m_capacity{0};
            std::vector<ParticleSnapshot> m_particles;
            std::vector<size_t> m_free;
            size_t m_alive{0};
            size_t m_emitted{0};
            std::mt19937 m_rng;
            float m_emissionAccumulator{0.0F};
            float m_burstAccumulator{0.0F};
            bool m_burstFired{false};
    };
} // namespace FIFE

#endif
