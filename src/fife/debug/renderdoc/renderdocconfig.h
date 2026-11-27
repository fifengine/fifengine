// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_RENDERDOC_CONFIG_H
#define FIFE_DEBUG_RENDERDOC_CONFIG_H

#include <string>

#include "platform.h"

namespace FIFE
{
    /** Settings for the RenderDoc debugger module.
     *
     * Defaults keep FIFE's behaviour unchanged: the module is off until a
     * host explicitly opts in through EngineSettings.
     */
    struct FIFE_API RenderDocConfig
    {
            bool enableRenderDoc{false};
            bool autoCapture{false};
            bool captureOnHotkey{true};
            std::string captureHotkey{"F12"};
            std::string captureDirectory;
            bool enableDebugMarkers{true};
            bool enableResourceNaming{true};
            bool logCaptureEvents{true};
    };
} // namespace FIFE

#endif
