// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_DEBUG_DIAGNOSTICS_PHASES_H
#define FIFE_DEBUG_DIAGNOSTICS_PHASES_H

namespace FIFE
{
    /** One naming table for RenderDoc groups, Tracy CPU zones and GPU zones.
     *
     * All values are string literals with static storage, which is required by
     * Tracy's zone macros and by RenderDoc's debug groups.
     */
    inline constexpr char const * kPhaseFrame        = "Frame";
    inline constexpr char const * kPhaseOffscreen    = "Offscreen Pass";
    inline constexpr char const * kPhaseTarget       = "Target Pass";
    inline constexpr char const * kPhaseWorld        = "World Pass";
    inline constexpr char const * kPhaseGui          = "UI Pass";
    inline constexpr char const * kPhaseCursor       = "Cursor Pass";
    inline constexpr char const * kPhaseMovie        = "Movie Pass";
    inline constexpr char const * kPhaseInput        = "Input";
    inline constexpr char const * kPhaseResourceLoad = "Resource Load";
} // namespace FIFE

#endif
