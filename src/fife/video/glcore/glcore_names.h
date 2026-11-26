// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_GLCORE_NAMES_H
#define FIFE_GLCORE_NAMES_H

#include <string>

#include "platform.h"

namespace FIFE
{
    FIFE_API bool isGlDepthBackend(std::string const & backendName);
    FIFE_API bool usesOpenGLRenderTargetClip(std::string const & backendName);
    FIFE_API bool usesGLRgba32UploadFormat(std::string const & backendName);
    FIFE_API bool bindsColorLikeSDL(std::string const & backendName);
    FIFE_API bool isCompiledRenderBackend(std::string const & backendName);
    FIFE_API std::string resolveCompiledBackendName(std::string const & requested);
} // namespace FIFE

#endif
