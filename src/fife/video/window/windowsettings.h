// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_VIDEO_WINDOW_WINDOWSETTINGS_H
#define FIFE_VIDEO_WINDOW_WINDOWSETTINGS_H

#include <string>

namespace FIFE
{

    /**
     * How the window is presented.
     *
     *  - Windowed: decorated window at the requested size.
     *  - Fullscreen: desktop fullscreen (no display-mode switch).
     *  - Borderless: undecorated window at the requested size (not fullscreen).
     */
    enum class WindowMode
    {
        Windowed   = 0,
        Fullscreen = 1,
        Borderless = 2
    };

    enum class ScalingMode
    {
        Linear  = 0,
        Integer = 1,
        Nearest = 2
    };

    struct WindowSettings
    {
            int width         = 1280;
            int height        = 720;
            /** 0-based display index, not an SDL_DisplayID. */
            int display       = 0;
            int x             = -1;
            int y             = -1;
            std::string title = "FIFE Application";
            std::string icon;
            bool resizable          = true;
            bool vsync              = true;
            bool opengl             = true;
            WindowMode windowMode   = WindowMode::Windowed;
            ScalingMode scalingMode = ScalingMode::Linear;
    };

    inline bool operator==(WindowSettings const & a, WindowSettings const & b)
    {
        return a.width == b.width && a.height == b.height && a.display == b.display && a.x == b.x && a.y == b.y &&
               a.title == b.title && a.icon == b.icon && a.resizable == b.resizable && a.vsync == b.vsync &&
               a.opengl == b.opengl && a.windowMode == b.windowMode && a.scalingMode == b.scalingMode;
    }

    /**
     * @return true if applying @p requested to a live window needs a restart.
     *
     * Size, position, mode, title, icon, VSync, resizable and scaling mode can
     * be applied to the existing window. A different monitor or a different
     * renderer (OpenGL vs. SDL) requires a new window/renderer and therefore a
     * restart; callers must not assume those fields were applied.
     */
    inline bool windowSettingsRequireRestart(WindowSettings const & current, WindowSettings const & requested)
    {
        return current.display != requested.display || current.opengl != requested.opengl;
    }

} // namespace FIFE

#endif
