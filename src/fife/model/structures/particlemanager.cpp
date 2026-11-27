// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "particlemanager.h"

// Standard C++ library includes
#include <algorithm>
#include <utility>
#include <vector>

// FIFE includes
#include "util/time/timemanager.h"

namespace FIFE
{
    ParticleManager::ParticleManager() : TimeEvent(0)
    {
        TimeManager::instance()->registerEvent(this);
        resetClock();
    }

    ParticleManager::~ParticleManager()
    {
        if (TimeManager::exists()) {
            TimeManager::instance()->unregisterEvent(this);
        }
    }

    ParticleManager* ParticleManager::getInstance()
    {
        return instance();
    }

    ParticleEmitter* ParticleManager::createEmitter(ParticleConfig const & config)
    {
        return adoptEmitter(std::make_unique<ParticleEmitter>(config));
    }

    ParticleEmitter* ParticleManager::adoptEmitter(std::unique_ptr<ParticleEmitter> emitter)
    {
        ParticleEmitter* const ptr = emitter.get();
        m_emitters.push_back(std::move(emitter));
        // ptr stays valid: ownership was just moved into the member container m_emitters.
        // cppcheck-suppress danglingLifetime
        m_emitters_view.push_back(ptr);
        // cppcheck-suppress returnDanglingLifetime
        return ptr;
    }

    void ParticleManager::removeEmitter(ParticleEmitter* emitter)
    {
        if (std::ranges::find(m_pending_removal, emitter) == m_pending_removal.end()) {
            m_pending_removal.push_back(emitter);
        }
    }

    void ParticleManager::flushRemovals()
    {
        for (ParticleEmitter const * victim : m_pending_removal) {
            auto const view = std::ranges::find(m_emitters_view, victim);
            if (view == m_emitters_view.end()) {
                continue;
            }
            auto const owner = std::ranges::find_if(m_emitters, [victim](std::unique_ptr<ParticleEmitter> const & e) {
                return e.get() == victim;
            });
            if (owner != m_emitters.end()) {
                m_emitters.erase(owner);
            }
            m_emitters_view.erase(view);
        }
        m_pending_removal.clear();
    }

    void ParticleManager::resetClock()
    {
        setLastUpdateTime64(TimeManager::instance()->now64());
    }

    void ParticleManager::setSeed(uint32_t seed)
    {
        for (ParticleEmitter* emitter : m_emitters_view) {
            emitter->setSeed(seed);
        }
    }

    void ParticleManager::updateEvent(uint32_t time)
    {
        constexpr double fixed_step = 1.0 / 120.0;
        m_accumulator += static_cast<double>(std::min(time, 50U)) / 1000.0;

        while (m_accumulator >= fixed_step) {
            // A snapshot keeps iteration valid if an emitter creates or removes
            // another one from inside update().
            std::vector<ParticleEmitter*> const snapshot = m_emitters_view;
            for (ParticleEmitter* emitter : snapshot) {
                emitter->update(static_cast<float>(fixed_step));
            }
            m_accumulator -= fixed_step;
            ++m_simulationStep;
        }

        flushRemovals();
    }
} // namespace FIFE
