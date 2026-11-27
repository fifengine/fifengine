// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_DIAGNOSTICS_CONFIG_H
#define FIFE_DEBUG_DIAGNOSTICS_CONFIG_H

#include "debug/renderdoc/renderdocconfig.h"
#include "platform.h"

namespace FIFE
{
    /** Runtime settings for the Tracy profiler module. */
    struct FIFE_API TracyConfig
    {
            bool enableTracy{false};
            bool enableGPUProfiling{true};
            bool enableMemoryProfiling{true};
    };

    /** Combined configuration owned by DiagnosticsManager. */
    struct FIFE_API DiagnosticsConfig
    {
            RenderDocConfig renderDoc;
            TracyConfig tracy;
    };
} // namespace FIFE

#endif
