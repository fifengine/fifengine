// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_BRIDGE_H
#define FIFE_GLCORE_BRIDGE_H

// Loader-neutral: neither GLEW nor glad headers belong here.
#include <cstdint>

#include "platform.h"
#include "util/structures/rect.h"

namespace FIFE
{
    /** Forward a batched image quad to the active RenderBackend.
     * Implemented in renderbackendgl.cpp so glcoreimage.cpp stays glad-only.
     */
    void glcoreAddImageToArray(uint32_t id, Rect const & rect, float const * st, uint8_t alpha, uint8_t const * rgba);
    void glcoreAddImageToArrayOverlay(
        Rect const & rect,
        uint32_t id1,
        float const * st1,
        uint32_t id2,
        float const * st2,
        uint8_t alpha,
        uint8_t const * rgba);
} // namespace FIFE

#endif
