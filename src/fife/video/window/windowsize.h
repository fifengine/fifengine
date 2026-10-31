// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_WINDOW_WINDOWSIZE_H
#define FIFE_VIDEO_WINDOW_WINDOWSIZE_H

// Standard C++ library includes
#include <algorithm>

namespace FIFE
{

    /** A window size in window coordinates (pixels on Windows, points on macOS). */
    struct WindowSize
    {
            int width  = 0;
            int height = 0;
    };

    /** Clamp a requested window size so it fits the desktop.
     *
     * SDL3 reports SDL_GetDisplayBounds() in screen coordinates, i.e. the same
     * coordinate space as SDL_GetWindowSize()/SDL_SetWindowSize(). That holds
     * on every platform even though the underlying units differ:
     *
     * - Windows: window coordinates are physical pixels (PerMonitorV2), and
     *   WIN_GetDisplayBounds() returns the raw GetMonitorInfo() pixels.
     * - macOS: window coordinates are points; Cocoa_GetDisplayBounds() and the
     *   display mode are in points, with SDL_DisplayMode::pixel_density
     *   converting a mode to pixels.
     * - X11/Wayland: pixels, content scale 1.0.
     *
     * SDL_GetDisplayContentScale()/SDL_GetWindowDisplayScale() describe the
     * expected UI scale for the DPI setting. They are *not* a conversion
     * between these coordinate spaces and must not be applied here: a
     * 1920x1080 window on a 1920x1080 desktop is valid at 125% scaling.
     *
     * @param boundsWidth/boundsHeight Display bounds as reported by
     *        SDL_GetDisplayBounds(), in window coordinates.
     */
    inline WindowSize clampWindowSizeToDesktop(
        int requestedWidth, int requestedHeight, int boundsWidth, int boundsHeight)
    {
        int const maxWidth  = std::max(1, boundsWidth);
        int const maxHeight = std::max(1, boundsHeight);

        return {std::clamp(requestedWidth, 1, maxWidth), std::clamp(requestedHeight, 1, maxHeight)};
    }

} // namespace FIFE

#endif
