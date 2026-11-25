// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Corresponding header include
#include "particlerenderer.h"

// Standard C++ library includes
#include <algorithm>
#include <cmath>
#include <memory>

// FIFE includes
#include "model/structures/particlemanager.h"
#include "video/image.h"
#include "video/iparticlerenderbackend.h"
#include "view/camera.h"

namespace FIFE
{
    ParticleRenderer::ParticleRenderer(RenderBackend* renderbackend, int32_t position) :
        RendererBase(renderbackend, position)
    {
        setEnabled(false);
    }

    ParticleRenderer::ParticleRenderer(ParticleRenderer const & old) : RendererBase(old)
    {
        setEnabled(false);
    }

    std::unique_ptr<RendererBase> ParticleRenderer::clone()
    {
        return std::make_unique<ParticleRenderer>(*this);
    }

    void ParticleRenderer::render(Camera* cam, Layer* layer, RenderList& instances)
    {
        static_cast<void>(instances);
        if (!ParticleManager::exists() || cam == nullptr || layer == nullptr) {
            return;
        }

        ParticleManager* const manager       = ParticleManager::instance();
        IParticleRenderBackend* const bridge = activeParticleRenderBackend();
        FIFE::Rect const viewport            = cam->getViewPort();
        float const w                        = static_cast<float>(std::max(1, viewport.w));
        float const h                        = static_cast<float>(std::max(1, viewport.h));
        float const viewProjection[16]       = {
            2.0F / w, 0.0F, 0.0F, 0.0F, 0.0F, -2.0F / h, 0.0F, 0.0F, 0.0F, 0.0F, 1.0F, 0.0F, -1.0F, 1.0F, 0.0F, 1.0F};
        for (ParticleEmitter* emitter : manager->getEmitters()) {
            if (emitter == nullptr || !emitter->isDrawnOn(layer)) {
                continue;
            }
            if (bridge != nullptr && emitter->isComputeEnabled()) {
                bridge->dispatch(
                    emitter->getId(), emitter->getAliveEstimate(), 1.0F / 120.0F, manager->getSimulationStep());
                if (emitter->getFrame(0)) {
                    bridge->draw(
                        emitter->getId(), emitter->getAliveEstimate(), *emitter->getFrame(0), viewProjection, viewport);
                }
                continue;
            }
            for (size_t slot = 0U; slot < emitter->getCapacity(); ++slot) {
                ParticleSnapshot const particle = emitter->getParticle(slot);
                if (!particle.alive) {
                    continue;
                }
                ImagePtr const frame = emitter->getFrame(particle.frame);
                if (!frame) {
                    continue;
                }
                ExactModelCoordinate const world = emitter->getParticleWorldPosition(slot);
                ScreenPoint const screen         = cam->toScreenCoordinates(world);
                int32_t const size = std::max<int32_t>(1, static_cast<int32_t>(std::lround(particle.size)));
                Rect const rect(screen.x - size / 2, screen.y - size / 2, size, size);
                frame->render(rect, particle.rgba[3], particle.rgba);
            }
        }
    }

    ParticleRenderer* ParticleRenderer::getInstance(IRendererContainer* cnt)
    {
        return dynamic_cast<ParticleRenderer*>(cnt->getRenderer("ParticleRenderer"));
    }
} // namespace FIFE
