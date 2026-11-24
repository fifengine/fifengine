// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "particleemitter.h"

// Standard C++ library includes
#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>

// FIFE includes
#include "model/structures/instance.h"
#include "model/structures/layer.h"

namespace FIFE
{
    namespace
    {
        class EmitterDeleteListener : public InstanceDeleteListener
        {
            public:
                explicit EmitterDeleteListener(ParticleEmitter* owner) : m_owner(owner)
                {
                }

                void onInstanceDeleted(Instance* instance) override
                {
                    m_owner->handleInstanceDeleted(instance);
                }

            private:
                ParticleEmitter* m_owner;
        };

        float uniform(std::mt19937 & rng, float a, float b)
        {
            std::uniform_real_distribution<float> dist(std::min(a, b), std::max(a, b));
            return dist(rng);
        }
    } // namespace

    ParticleEmitter::ParticleEmitter(ParticleConfig const & config) : m_config(config), m_rng(1U)
    {
        rebuildPool();
    }

    ParticleEmitter::~ParticleEmitter()
    {
        detach();
    }

    uint32_t ParticleEmitter::effectiveCapacity() const
    {
        uint32_t cap = m_config.maxParticles;
        if (cap == 0U) {
            double derived = std::ceil(static_cast<double>(m_config.emissionRate) *
                                       static_cast<double>(m_config.lifetimeMax)) +
                             static_cast<double>(m_config.burstCount);
            derived = std::clamp(derived, 0.0, 2048.0);
            cap = static_cast<uint32_t>(derived);
        }
        return std::min(cap, 2048U);
    }

    void ParticleEmitter::rebuildPool()
    {
        m_capacity = effectiveCapacity();
        m_particles.assign(m_capacity, ParticleSnapshot{});
        m_free.clear();
        m_free.reserve(m_capacity);
        for (size_t i = m_capacity; i > 0U; --i) {
            m_free.push_back(i - 1U);
        }
        m_alive                = 0;
        m_emitted              = 0;
        m_emissionAccumulator  = 0.0F;
        m_burstAccumulator     = 0.0F;
        m_burstFired           = false;
    }

    void ParticleEmitter::setSeed(uint32_t seed)
    {
        m_rng.seed(seed);
    }

    uint32_t ParticleEmitter::getMaxParticles() const
    {
        return m_capacity;
    }

    void ParticleEmitter::setMaxParticles(uint32_t maxParticles)
    {
        m_config.maxParticles = maxParticles;
        rebuildPool();
    }

    bool ParticleEmitter::spawn()
    {
        if (m_free.empty()) {
            return false;
        }

        size_t const slot = m_free.back();
        m_free.pop_back();
        ParticleSnapshot & p = m_particles[slot];
        p                    = ParticleSnapshot{};
        p.alive              = true;
        p.lifetime           = std::max(0.001F, uniform(m_rng, m_config.lifetimeMin, m_config.lifetimeMax));
        float const speed    = uniform(m_rng, m_config.speedMin, m_config.speedMax);
        p.vx                 = speed;
        p.vy                 = 0.0F;

        switch (m_config.shape) {
            case ParticleShape::Point:
                break;
            case ParticleShape::Box:
                p.x = uniform(m_rng, -m_config.shapeWidth / 2.0F, m_config.shapeWidth / 2.0F);
                p.y = uniform(m_rng, -m_config.shapeHeight / 2.0F, m_config.shapeHeight / 2.0F);
                break;
            case ParticleShape::Circle: {
                float const angle  = uniform(m_rng, 0.0F, 2.0F * std::numbers::pi_v<float>);
                float const radius = std::sqrt(uniform(m_rng, 0.0F, 1.0F)) *
                                     std::min(m_config.shapeWidth, m_config.shapeHeight) / 2.0F;
                p.x = std::cos(angle) * radius;
                p.y = std::sin(angle) * radius;
                break;
            }
        }

        if (!m_config.localSpace) {
            ExactModelCoordinate const node = getNodeWorldPosition();
            p.x += static_cast<float>(node.x);
            p.y += static_cast<float>(node.y);
        }

        refreshVisuals(p);
        ++m_alive;
        ++m_emitted;
        return true;
    }

    void ParticleEmitter::refreshVisuals(ParticleSnapshot & particle) const
    {
        float t = particle.lifetime > 0.0F ? particle.age / particle.lifetime : 0.0F;
        t                  = std::clamp(t, 0.0F, 1.0F);
        particle.size      = m_config.startSize + (m_config.endSize - m_config.startSize) * t;
        for (size_t i = 0U; i < 4U; ++i) {
            float const channel = static_cast<float>(m_config.startColor[i]) +
                                  (static_cast<float>(m_config.endColor[i]) -
                                   static_cast<float>(m_config.startColor[i])) *
                                      t;
            particle.rgba[i] = static_cast<uint8_t>(std::lround(channel));
        }
        particle.frame = (m_config.frameCount != 0U && m_config.frameDuration > 0.0F)
                             ? static_cast<uint32_t>(particle.age / m_config.frameDuration) % m_config.frameCount
                             : 0U;
    }

    void ParticleEmitter::update(float dt)
    {
        if (dt <= 0.0F) {
            return;
        }

        bool const mayEmit = m_emission_enabled && (m_config.looping || !m_burstFired);

        if (m_config.emissionRate > 0.0F && mayEmit) {
            m_emissionAccumulator += m_config.emissionRate * dt;
            while (m_emissionAccumulator >= 1.0F) {
                if (!spawn()) {
                    m_emissionAccumulator = 0.0F;
                    break;
                }
                m_emissionAccumulator -= 1.0F;
            }
        }

        if (m_config.burstCount > 0U && m_config.burstInterval > 0.0F && mayEmit) {
            m_burstAccumulator += dt;
            while (m_burstAccumulator >= m_config.burstInterval) {
                for (uint32_t i = 0U; i < m_config.burstCount; ++i) {
                    if (!spawn()) {
                        break;
                    }
                }
                m_burstAccumulator -= m_config.burstInterval;
                m_burstFired = true;
            }
        }

        float const damping = std::max(0.0F, 1.0F - m_config.drag * dt);
        for (size_t slot = 0U; slot < m_particles.size(); ++slot) {
            ParticleSnapshot & p = m_particles[slot];
            if (!p.alive) {
                continue;
            }
            p.vx *= damping;
            p.vy *= damping;
            p.vy += m_config.gravity * dt;
            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.age += dt;
            if (p.age >= p.lifetime) {
                p.alive = false;
                --m_alive;
                m_free.push_back(slot);
            } else {
                refreshVisuals(p);
            }
        }
    }

    ParticleSnapshot ParticleEmitter::getParticle(size_t slot) const
    {
        return m_particles.at(slot);
    }

    ExactModelCoordinate ParticleEmitter::getParticleWorldPosition(size_t slot) const
    {
        ParticleSnapshot const & p = m_particles.at(slot);
        if (m_config.localSpace) {
            ExactModelCoordinate const node = getNodeWorldPosition();
            return ExactModelCoordinate(node.x + static_cast<double>(p.x), node.y + static_cast<double>(p.y), 0.0);
        }
        return ExactModelCoordinate(static_cast<double>(p.x), static_cast<double>(p.y), 0.0);
    }

    void ParticleEmitter::detach()
    {
        if (m_attached_instance != nullptr && m_delete_listener != nullptr) {
            m_attached_instance->removeDeleteListener(m_delete_listener.get());
        }
        m_delete_listener.reset();
        m_attached_instance = nullptr;
        m_node.setAttached(Point(0, 0));
        m_drawn_layer       = nullptr;
        m_has_last_world    = false;
        m_emission_enabled  = true;
    }

    void ParticleEmitter::attachToInstance(Instance* instance, Point const & offset)
    {
        detach();
        if (instance == nullptr) {
            return;
        }
        m_node.setAttached(instance, offset);
        m_attached_instance = instance;
        m_drawn_layer       = instance->getLocationRef().getLayer();
        m_delete_listener   = std::make_unique<EmitterDeleteListener>(this);
        instance->addDeleteListener(m_delete_listener.get());
        m_emission_enabled = true;
    }

    void ParticleEmitter::attachToLayer(Layer* layer, Point const & offset)
    {
        detach();
        if (layer == nullptr) {
            return;
        }
        m_node.setAttached(layer);
        m_node.setRelative(offset);
        m_drawn_layer      = layer;
        m_emission_enabled = true;
    }

    void ParticleEmitter::attachToPoint(Point const & point)
    {
        detach();
        m_node.setAttached(point);
        m_emission_enabled = true;
    }

    bool ParticleEmitter::isDrawnOn(Layer const * layer) const
    {
        // Unattached (screen-space) emitters follow RendererNode's convention
        // and are drawn on every active layer. In a multi-layer scene this
        // means one render pass per layer; attach to a layer to restrict it.
        return m_drawn_layer == nullptr || m_drawn_layer == layer;
    }

    ExactModelCoordinate ParticleEmitter::getNodeWorldPosition() const
    {
        if (m_has_last_world) {
            return m_last_world;
        }

        ExactModelCoordinate result{0.0, 0.0, 0.0};
        if (m_attached_instance != nullptr) {
            result = m_attached_instance->getLocationRef().getMapCoordinates();
        } else {
            Location const attached = m_node.getAttachedLocation();
            if (attached.isValid()) {
                result = attached.getMapCoordinates();
            } else {
                Point const point = m_node.getPoint();
                return ExactModelCoordinate(static_cast<double>(point.x), static_cast<double>(point.y), 0.0);
            }
        }

        Location const relative = m_node.getOffsetLocation();
        if (relative.isValid()) {
            ExactModelCoordinate const offset = relative.getMapCoordinates();
            result.x += offset.x;
            result.y += offset.y;
        }
        Point const relativePoint = m_node.getOffsetPoint();
        result.x += static_cast<double>(relativePoint.x);
        result.y += static_cast<double>(relativePoint.y);
        return result;
    }

    void ParticleEmitter::handleInstanceDeleted(Instance* instance)
    {
        if (instance != m_attached_instance) {
            return;
        }
        m_last_world = instance->getLocationRef().getMapCoordinates();
        Location const relative = m_node.getOffsetLocation();
        if (relative.isValid()) {
            ExactModelCoordinate const offset = relative.getMapCoordinates();
            m_last_world.x += offset.x;
            m_last_world.y += offset.y;
        }
        Point const relativePoint = m_node.getOffsetPoint();
        m_last_world.x += static_cast<double>(relativePoint.x);
        m_last_world.y += static_cast<double>(relativePoint.y);

        m_has_last_world    = true;
        m_emission_enabled  = false;
        m_attached_instance = nullptr;
        // Do not touch the RendererNode here: its own delete listener clears
        // the instance reference with listener=false, and mutating the
        // instance's listener list from inside Instance::~Instance would
        // invalidate the vector being iterated. Keep our listener object
        // alive until detach(), for the same reason.
    }

    void ParticleEmitter::setFrames(std::vector<ImagePtr> frames)
    {
        m_frames = std::move(frames);
    }

    void ParticleEmitter::addFrame(ImagePtr frame)
    {
        m_frames.push_back(std::move(frame));
    }

    ImagePtr const & ParticleEmitter::getFrame(uint32_t frame) const
    {
        static ImagePtr const empty;
        if (m_frames.empty()) {
            return empty;
        }
        return m_frames[static_cast<size_t>(frame) % m_frames.size()];
    }
} // namespace FIFE
