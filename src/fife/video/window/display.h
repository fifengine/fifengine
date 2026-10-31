// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_WINDOW_DISPLAY_H
#define FIFE_VIDEO_WINDOW_DISPLAY_H

#include <SDL3/SDL.h>

#include <string>
#include <vector>

#include "platform.h"

namespace FIFE
{

    /**
     * @brief Display enumeration and query class.
     *
     * Provides static methods to enumerate displays and query their properties:
     * count, name, bounds, content scale, and modes.
     *
     * For window-level DPI calculation, prefer Window::getDPIScaleFactor()
     * which uses the ratio of SDL_GetWindowSizeInPixels() / SDL_GetWindowSize().
     * SDL_GetDisplayContentScale() reports the OS-level content scale hint.
     *
     * Usage example:
     * @code
     * int count = Display::getDisplayCount();
     * float scale = Display::getDisplayContentScale(0);
     * int w = Display::getDisplayWidth(0);
     * @endcode
     */
    class FIFE_API Display
    {
        public:
            static int getDisplayCount();
            /** Map a 0-based display index to the SDL_DisplayID used by
             *  SDL_WINDOWPOS_CENTERED_DISPLAY and the display APIs.
             *  @return 0 if the index is out of range (SDL uses 0 for errors).
             */
            static SDL_DisplayID getDisplayID(int index);
            static std::string getDisplayName(int index);
            /** Full display bounds in SDL screen coordinates (pixels on
             *  Windows, points on macOS). */
            static SDL_Rect getDisplayBounds(int index);
            /** Desktop work area for the display, excluding taskbars/docks.
             *  Use this for windowed placement; use getDisplayBounds() for
             *  fullscreen. Same coordinate space as getDisplayBounds(). */
            static SDL_Rect getUsableBounds(int index);
            static float getDisplayContentScale(int index);
            static int getDisplayWidth(int index);
            static int getDisplayHeight(int index);
            static SDL_DisplayMode getDesktopMode(int index);
            static std::vector<SDL_DisplayMode> getSupportedModes(int index);
            static SDL_DisplayMode getClosestMode(int index, int width, int height, int refreshRate);
    };

} // namespace FIFE

#endif
