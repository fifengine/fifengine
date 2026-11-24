// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PARTICLERENDERER_H
#define FIFE_PARTICLERENDERER_H

#include "platform.h"

#include "view/rendererbase.h"

#include <memory>
#include <string>

namespace FIFE
{
    class ParticleRenderer : public RendererBase
    {
        public:
            ParticleRenderer(RenderBackend* renderbackend, int32_t position);
            ParticleRenderer(ParticleRenderer const & old);
            ParticleRenderer & operator=(ParticleRenderer const &) = delete;
            ~ParticleRenderer() override                           = default;

            std::unique_ptr<RendererBase> clone() override;
            void render(Camera* cam, Layer* layer, RenderList& instances) override;
            std::string getName() override
            {
                return "ParticleRenderer";
            }
            void reset() override
            {
            }

            static ParticleRenderer* getInstance(IRendererContainer* cnt);
    };
} // namespace FIFE

#endif
