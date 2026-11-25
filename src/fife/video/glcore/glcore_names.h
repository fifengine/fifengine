// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_NAMES_H
#define FIFE_GLCORE_NAMES_H

#include <string>

#include "platform.h"

namespace FIFE
{
    bool isGlDepthBackend(std::string const & backendName);
    bool usesOpenGLRenderTargetClip(std::string const & backendName);
    bool usesGLRgba32UploadFormat(std::string const & backendName);
    bool bindsColorLikeSDL(std::string const & backendName);
    bool isCompiledRenderBackend(std::string const & backendName);
    std::string resolveCompiledBackendName(std::string const & requested);
} // namespace FIFE

#endif
