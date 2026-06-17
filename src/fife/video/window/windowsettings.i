// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "video/window/windowsettings.h"
%}

namespace FIFE {

enum class WindowMode {
    Windowed = 0,
    Fullscreen = 1,
    Borderless = 2
};

enum class ScalingMode {
    Linear = 0,
    Integer = 1,
    Nearest = 2
};

struct WindowSettings {
    int width;
    int height;
    int display;
    int x;
    int y;
    std::string title;
    std::string icon;
    bool resizable;
    bool vsync;
    bool opengl;
    WindowMode windowMode;
    ScalingMode scalingMode;
};

%extend WindowSettings {
    bool __eq__(const WindowSettings& other) const {
        return (*$self) == other;
    }
};

}
