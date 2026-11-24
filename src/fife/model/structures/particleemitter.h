// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PARTICLEEMITTER_H
#define FIFE_PARTICLEEMITTER_H

#include "platform.h"

#include "model/metamodel/modelcoords.h"
#include "model/structures/particleconfig.h"
#include "model/structures/renderernode.h"
#include "video/image.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace FIFE
{
    class Instance;
    class Layer;

    class ParticleEmitter
    {
        public:
            explicit ParticleEmitter(ParticleConfig const & config);
            virtual ~ParticleEmitter();

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

            void attachToInstance(Instance* instance, Point const & offset = Point(0, 0));
            void attachToLayer(Layer* layer, Point const & offset = Point(0, 0));
            void attachToPoint(Point const & point);
            void detach();
            bool isDrawnOn(Layer const * layer) const;
            bool isEmissionEnabled() const
            {
                return m_emission_enabled;
            }
            Instance* getAttachedInstance() const
            {
                return m_attached_instance;
            }
            ExactModelCoordinate getNodeWorldPosition() const;
            void handleInstanceDeleted(Instance* instance);

            void setFrames(std::vector<ImagePtr> frames);
            void addFrame(ImagePtr frame);
            size_t getFrameCount() const
            {
                return m_frames.size();
            }
            ImagePtr const & getFrame(uint32_t frame) const;

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

            // RendererNode's query methods are not const-qualified.
            mutable RendererNode m_node{Point(0, 0)};
            Layer* m_drawn_layer{nullptr};
            Instance* m_attached_instance{nullptr};
            std::unique_ptr<InstanceDeleteListener> m_delete_listener;
            ExactModelCoordinate m_last_world{0.0, 0.0, 0.0};
            bool m_has_last_world{false};
            bool m_emission_enabled{true};
            std::vector<ImagePtr> m_frames;
    };
} // namespace FIFE

#endif
