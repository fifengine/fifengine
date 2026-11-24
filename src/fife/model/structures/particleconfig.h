// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_PARTICLECONFIG_H
#define FIFE_PARTICLECONFIG_H

#include "platform.h"

#include <cstdint>

namespace FIFE
{
    enum class ParticleShape : uint8_t
    {
        Point,
        Box,
        Circle
    };

    struct ParticleConfig
    {
        float emissionRate{0.0F};
        uint32_t burstCount{0};
        float burstInterval{0.0F};
        ParticleShape shape{ParticleShape::Point};
        float shapeWidth{0.0F};
        float shapeHeight{0.0F};
        float lifetimeMin{1.0F};
        float lifetimeMax{1.0F};
        float speedMin{0.0F};
        float speedMax{0.0F};
        float gravity{0.0F};
        float drag{0.0F};
        float startSize{1.0F};
        float endSize{1.0F};
        uint8_t startColor[4]{255, 255, 255, 255};
        uint8_t endColor[4]{255, 255, 255, 255};
        uint32_t frameCount{0};
        float frameDuration{0.0F};
        bool localSpace{false};
        uint32_t maxParticles{0}; // 0 => derive from rate, lifetime and burst
        bool looping{true};
    };

    struct ParticleSnapshot
    {
        bool alive{false};
        float x{0.0F};
        float y{0.0F};
        float vx{0.0F};
        float vy{0.0F};
        float age{0.0F};
        float lifetime{1.0F};
        float size{1.0F};
        uint8_t rgba[4]{255, 255, 255, 255};
        uint32_t frame{0};
    };
} // namespace FIFE

#endif
