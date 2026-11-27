// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#include "iparticlerenderbackend.h"

#include "video/renderbackend.h"

namespace FIFE
{
    IParticleRenderBackend* activeParticleRenderBackend()
    {
        if (!RenderBackend::exists()) {
            return nullptr;
        }
        return RenderBackend::instance()->getParticleRenderBackend();
    }
} // namespace FIFE
