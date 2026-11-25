// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PARTICLEMANAGER_H
#define FIFE_PARTICLEMANAGER_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "model/structures/particleemitter.h"
#include "platform.h"
#include "util/base/singleton.h"
#include "util/time/timeevent.h"

namespace FIFE
{
    class FIFE_API ParticleManager : public TimeEvent, public DynamicSingleton<ParticleManager>
    {
        public:
            ParticleManager();
            ~ParticleManager() override;

            ParticleManager(ParticleManager const &)            = delete;
            ParticleManager& operator=(ParticleManager const &) = delete;

            static ParticleManager* getInstance();

            ParticleEmitter* createEmitter(ParticleConfig const & config);
            ParticleEmitter* adoptEmitter(std::unique_ptr<ParticleEmitter> emitter);
            void removeEmitter(ParticleEmitter* emitter);

            size_t getEmitterCount() const
            {
                return m_emitters.size();
            }

            std::vector<ParticleEmitter*> const & getEmitters() const
            {
                return m_emitters_view;
            }

            void resetClock();
            void setSeed(uint32_t seed);
            void updateEvent(uint32_t time) override;

        private:
            void flushRemovals();

            std::vector<std::unique_ptr<ParticleEmitter>> m_emitters;
            std::vector<ParticleEmitter*> m_emitters_view;
            std::vector<ParticleEmitter*> m_pending_removal;
            double m_accumulator{0.0};
    };
} // namespace FIFE

#endif
