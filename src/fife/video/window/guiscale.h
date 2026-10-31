// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_WINDOW_GUISCALE_H
#define FIFE_VIDEO_WINDOW_GUISCALE_H

#include <algorithm>

#include "platform.h"

namespace FIFE
{

    /** GUI content is never shrunk below its configured size. */
    inline constexpr float GUI_SCALE_MIN = 1.0F;
    /** Upper bound for implausible OS/display values. */
    inline constexpr float GUI_SCALE_MAX = 8.0F;

    /**
     * Calculates the scale factor the GUI should apply
     * to fonts and widget metrics.
     *
     *  Window::getDPIScaleFactor() is the pixel/point ratio that the font
     *  renderer already applies. Window::getDisplayScale() is the operating
     *  system's UI/content scale. On Windows both window sizes are physical
     *  pixels, so the ratio is 1.0 while the content scale carries the DPI
     *  percentage. On macOS Retina the ratio is 2.0 and the content scale is
     *  also 2.0, so dividing prevents scaling fonts twice.
     *
     *  The result is clamped to [1.0, 8.0]: GUI content is never shrunk below
     *  its configured size, and implausible display values cannot explode
     *  font sizes.
     *
     *  @param displayScale Operating-system UI/content scale (1.0 = 100%).
     *  @param dpiScaleFactor Window pixel/point ratio (1.0 = no scaling).
     *  @param enabled False disables GUI scaling completely.
     *  @return Extra GUI scale factor.
     */
    inline float calculateGUIScale(float displayScale, float dpiScaleFactor, bool enabled)
    {
        if (!enabled) {
            return 1.0F;
        }
        if (!(displayScale > 0.0F)) {
            displayScale = 1.0F;
        }
        if (!(dpiScaleFactor > 0.0F)) {
            dpiScaleFactor = 1.0F;
        }
        float const scale = displayScale / dpiScaleFactor;
        return std::clamp(scale, GUI_SCALE_MIN, GUI_SCALE_MAX);
    }

} // namespace FIFE

#endif
