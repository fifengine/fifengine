// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <catch2/catch_test_macros.hpp>

// 3rd party library includes

// FIFE includes
#include "video/window/windowsettings.h"

using FIFE::ScalingMode;
using FIFE::WindowMode;
using FIFE::WindowSettings;

TEST_CASE("WindowSettings_DefaultValues")
{
    WindowSettings const s;
    CHECK(s.width == 1280);
    CHECK(s.height == 720);
    CHECK(s.display == 0);
    CHECK(s.x == -1);
    CHECK(s.y == -1);
    CHECK(s.title == "FIFE Application");
    CHECK(s.icon == "");
    CHECK(s.resizable == true);
    CHECK(s.vsync == true);
    CHECK(s.opengl == true);
    CHECK(s.windowMode == WindowMode::Windowed);
    CHECK(s.scalingMode == ScalingMode::Linear);
}

TEST_CASE("WindowSettings_Equality")
{
    WindowSettings a;
    WindowSettings b;
    CHECK(a == b);

    a.width = 640;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.height = 480;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.display = 1;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.x = 100;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.y = 200;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.title = "Custom";
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.icon = "icon.png";
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.resizable = false;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.vsync = false;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.opengl = false;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.windowMode = WindowMode::Fullscreen;
    CHECK_FALSE(a == b);
    a = WindowSettings{};

    a.scalingMode = ScalingMode::Integer;
    CHECK_FALSE(a == b);
}

TEST_CASE("WindowSettings_WindowModes")
{
    CHECK(static_cast<int>(WindowMode::Windowed) == 0);
    CHECK(static_cast<int>(WindowMode::Fullscreen) == 1);
    CHECK(static_cast<int>(WindowMode::Borderless) == 2);

    WindowSettings ws;
    ws.windowMode = WindowMode::Windowed;
    CHECK(ws.windowMode == WindowMode::Windowed);

    ws.windowMode = WindowMode::Fullscreen;
    CHECK(ws.windowMode == WindowMode::Fullscreen);

    ws.windowMode = WindowMode::Borderless;
    CHECK(ws.windowMode == WindowMode::Borderless);
}

TEST_CASE("WindowSettings_ScalingModes")
{
    CHECK(static_cast<int>(ScalingMode::Linear) == 0);
    CHECK(static_cast<int>(ScalingMode::Integer) == 1);
    CHECK(static_cast<int>(ScalingMode::Nearest) == 2);

    WindowSettings ws;
    ws.scalingMode = ScalingMode::Linear;
    CHECK(ws.scalingMode == ScalingMode::Linear);

    ws.scalingMode = ScalingMode::Integer;
    CHECK(ws.scalingMode == ScalingMode::Integer);

    ws.scalingMode = ScalingMode::Nearest;
    CHECK(ws.scalingMode == ScalingMode::Nearest);
}

TEST_CASE("WindowSettings_DisplayIndex")
{
    WindowSettings ws;
    CHECK(ws.display == 0);

    ws.display = 1;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.display == 1);

    ws.display = 5;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.display == 5);

    ws.display = -1;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.display == -1);
}

TEST_CASE("IntegerScale_CalculationLogic")
{
    auto integerScale = [](int displayW, int displayH, int gameW, int gameH) -> int {
        (void)displayH;
        (void)gameH;
        if (gameW == 0) {
            return 0;
        }
        int const scale = displayW / gameW;
        return scale > 0 ? scale : 0;
    };

    CHECK(integerScale(1920, 1080, 640, 480) == 3);
    CHECK(integerScale(1280, 720, 640, 480) == 2);
    CHECK(integerScale(800, 600, 640, 480) == 1);
    CHECK(integerScale(1920, 1080, 3840, 2160) == 0);
}

TEST_CASE("WindowSettings_Resizable")
{
    WindowSettings ws;
    CHECK(ws.resizable == true);

    ws.resizable = false;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.resizable == false);

    ws.resizable = true;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.resizable == true);
}

TEST_CASE("WindowSettings_Position")
{
    WindowSettings ws;
    CHECK(ws.x == -1);
    CHECK(ws.y == -1);

    ws.x = 100;
    ws.y = 200;
    CHECK(ws.x == 100);
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.y == 200);

    ws.x = -1;
    ws.y = -1;
    CHECK(ws.x == -1);
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.y == -1);
}

TEST_CASE("WindowSettings_VSync")
{
    WindowSettings ws;
    CHECK(ws.vsync == true);

    ws.vsync = false;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.vsync == false);

    ws.vsync = true;
    // cppcheck-suppress knownConditionTrueFalse
    CHECK(ws.vsync == true);
}
