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
            static std::string getDisplayName(int index);
            static SDL_Rect getDisplayBounds(int index);
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
