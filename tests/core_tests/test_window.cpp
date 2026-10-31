// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

// Standard C++ library includes
#include <stdexcept>
#include <string>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

// 3rd party library includes
#include <SDL3/SDL.h>

// FIFE includes
#include "video/window/display.h"
#include "video/window/guiscale.h"
#include "video/window/window.h"
#include "video/window/windowsettings.h"
#include "video/window/windowsize.h"

using Catch::Approx;
using FIFE::calculateGUIScale;
using FIFE::clampWindowSizeToDesktop;
using FIFE::ScalingMode;
using FIFE::WindowMode;
using FIFE::WindowSettings;
using FIFE::windowSettingsRequireRestart;

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

// ---------------------------------------------------------------------------
// Window size clamping (unknown-horizons#2923)
// ---------------------------------------------------------------------------

TEST_CASE("ClampWindowSizeToDesktop_RequestFits")
{
    // 1920x1080 on a 1920x1080 desktop: window coordinates are pixels on
    // Windows even at 125%, so this must NOT be clamped.
    auto const size = clampWindowSizeToDesktop(1920, 1080, 1920, 1080);
    CHECK(size.width == 1920);
    CHECK(size.height == 1080);
}

TEST_CASE("ClampWindowSizeToDesktop_RequestTooLarge")
{
    auto const size = clampWindowSizeToDesktop(2560, 1440, 1920, 1080);
    CHECK(size.width == 1920);
    CHECK(size.height == 1080);
}

TEST_CASE("ClampWindowSizeToDesktop_OneAxisTooLarge")
{
    auto const size = clampWindowSizeToDesktop(1920, 2000, 1920, 1080);
    CHECK(size.width == 1920);
    CHECK(size.height == 1080);
}

TEST_CASE("ClampWindowSizeToDesktop_SmallRequestUnchanged")
{
    auto const size = clampWindowSizeToDesktop(800, 600, 1920, 1080);
    CHECK(size.width == 800);
    CHECK(size.height == 600);
}

TEST_CASE("ClampWindowSizeToDesktop_InvalidBounds")
{
    auto const size = clampWindowSizeToDesktop(800, 600, 0, 0);
    CHECK(size.width == 1);
    CHECK(size.height == 1);
}

// ---------------------------------------------------------------------------
// GUI scale math (unknown-horizons#2923, WP-05)
// ---------------------------------------------------------------------------

TEST_CASE("CalculateGUIScale_PlatformCases")
{
    // No scaling at all.
    CHECK(calculateGUIScale(1.0F, 1.0F, true) == 1.0F);

    // Windows at 150%: window coordinates are physical pixels, so the
    // pixel/point ratio stays 1.0 and the content scale carries the DPI.
    CHECK(calculateGUIScale(1.5F, 1.0F, true) == Approx(1.5F));
    CHECK(calculateGUIScale(2.0F, 1.0F, true) == Approx(2.0F));

    // macOS Retina: points are already DPI-independent and the renderer
    // applies the 2x ratio. The GUI must not apply it a second time.
    CHECK(calculateGUIScale(2.0F, 2.0F, true) == 1.0F);

    // Opt-out forces identity regardless of platform values.
    CHECK(calculateGUIScale(1.5F, 1.0F, false) == 1.0F);
    CHECK(calculateGUIScale(2.0F, 2.0F, false) == 1.0F);
}

TEST_CASE("CalculateGUIScale_ClampsInvalidAndExtremeValues")
{
    // Unknown/failed display queries must not shrink or zero the GUI.
    CHECK(calculateGUIScale(0.0F, 1.0F, true) == 1.0F);
    CHECK(calculateGUIScale(-1.0F, 1.0F, true) == 1.0F);
    CHECK(calculateGUIScale(1.0F, 0.0F, true) == 1.0F);

    // A content scale below the pixel ratio is clamped to identity.
    CHECK(calculateGUIScale(1.0F, 2.0F, true) == 1.0F);

    // Guard against implausible values producing enormous fonts.
    CHECK(calculateGUIScale(100.0F, 1.0F, true) == 8.0F);
}

// ---------------------------------------------------------------------------
// Window creation: display pinning and mode semantics (WP-04)
// ---------------------------------------------------------------------------

namespace
{
    void ensureVideoInitialized()
    {
        if (SDL_GetCurrentVideoDriver() == nullptr) {
            SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "dummy");
        }
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        }
    }
} // namespace

TEST_CASE("Window_DisplayScaleIsPositiveAndResetsOnDestroy")
{
    ensureVideoInitialized();
    FIFE::Window window;

    // Before creation there is no display to query.
    CHECK(window.getDisplayScale() == 1.0F);

    WindowSettings settings;
    settings.width  = 800;
    settings.height = 600;
    settings.opengl = false;

    window.create(settings);
    // The dummy driver reports 1.0, but real drivers may report the OS
    // content scale (1.25, 1.5, 2.0, ...). It must never be zero/negative.
    CHECK(window.getDisplayScale() > 0.0F);

    window.destroy();
    CHECK(window.getDisplayScale() == 1.0F);
}

TEST_CASE("WindowCreate_InvalidDisplayFallsBackToFirst")
{
    ensureVideoInitialized();
    FIFE::Window window;
    WindowSettings settings;
    settings.width      = 800;
    settings.height     = 600;
    settings.opengl     = false;
    settings.windowMode = WindowMode::Windowed;
    settings.display    = 99;

    window.create(settings);

    // WindowSettings::display is an index in [0, displayCount), not an
    // SDL_DisplayID. An out-of-range value must not reach SDL.
    CHECK(window.getSettings().display == 0);
    window.destroy();
}

TEST_CASE("WindowCreate_FullscreenUsesDesktopModeOnTargetDisplay")
{
    ensureVideoInitialized();
    FIFE::Window window;
    WindowSettings settings;
    settings.width      = 800;
    settings.height     = 600;
    settings.opengl     = false;
    settings.windowMode = WindowMode::Fullscreen;
    settings.display    = 0;

    window.create(settings);
    window.show();

    CHECK(window.getFullscreenMode() == WindowMode::Fullscreen);
    auto const bounds = FIFE::Display::getDisplayBounds(0);
    CHECK(window.getWidthInPoints() == bounds.w);
    CHECK(window.getHeightInPoints() == bounds.h);
    // The requested windowed size is kept for leaving fullscreen.
    CHECK(window.getSettings().width == 800);
    CHECK(window.getSettings().height == 600);
    window.destroy();
}

TEST_CASE("WindowCreate_NegativePositionIsExplicit")
{
    ensureVideoInitialized();
    FIFE::Window window;
    WindowSettings settings;
    settings.width      = 800;
    settings.height     = 600;
    settings.opengl     = false;
    settings.windowMode = WindowMode::Windowed;
    settings.display    = 0;
    // A monitor left of/above the primary has negative coordinates. -1 is the
    // only "unspecified" value, so -500/-300 must reach SDL unchanged.
    settings.x = -500;
    settings.y = -300;

    window.create(settings);

    int x = 0;
    int y = 0;
    REQUIRE(SDL_GetWindowPosition(window.getSDLWindow(), &x, &y));
    CHECK(x == -500);
    CHECK(y == -300);
    window.destroy();
}

TEST_CASE("WindowMode_BorderlessRoundTripKeepsGeometry")
{
    ensureVideoInitialized();
    FIFE::Window window;
    WindowSettings settings;
    settings.width      = 1280;
    settings.height     = 720;
    settings.opengl     = false;
    settings.windowMode = WindowMode::Borderless;
    settings.display    = 0;
    settings.x          = -500;
    settings.y          = -300;

    window.create(settings);
    window.show();

    // Borderless is an undecorated window at the requested size, not
    // fullscreen (creation and setFullscreenMode must agree).
    CHECK(window.getFullscreenMode() == WindowMode::Borderless);
    CHECK(window.getWidthInPoints() == 1280);
    CHECK(window.getHeightInPoints() == 720);
    int beforeX = 0;
    int beforeY = 0;
    REQUIRE(SDL_GetWindowPosition(window.getSDLWindow(), &beforeX, &beforeY));
    CHECK(beforeX == -500);
    CHECK(beforeY == -300);

    window.setFullscreenMode(WindowMode::Fullscreen);
    CHECK(window.getFullscreenMode() == WindowMode::Fullscreen);
    auto const bounds = FIFE::Display::getDisplayBounds(0);
    CHECK(window.getWidthInPoints() == bounds.w);
    CHECK(window.getHeightInPoints() == bounds.h);

    window.setFullscreenMode(WindowMode::Borderless);
    CHECK(window.getFullscreenMode() == WindowMode::Borderless);
    CHECK(window.getWidthInPoints() == 1280);
    CHECK(window.getHeightInPoints() == 720);
    int afterX = 0;
    int afterY = 0;
    REQUIRE(SDL_GetWindowPosition(window.getSDLWindow(), &afterX, &afterY));
    CHECK(afterX == beforeX);
    CHECK(afterY == beforeY);
    window.destroy();
}

// ---------------------------------------------------------------------------
// Window lifetime
// ---------------------------------------------------------------------------

TEST_CASE("Window_DestroyNotifiesBeforeSDLWindowIsFreed")
{
    ensureVideoInitialized();
    FIFE::Window window;
    WindowSettings settings;
    settings.width      = 320;
    settings.height     = 200;
    settings.opengl     = false;
    settings.windowMode = WindowMode::Windowed;
    window.create(settings);

    SDL_Window* const handle = window.getSDLWindow();
    REQUIRE(handle != nullptr);
    int const expectedWidth  = window.getWidthInPoints();
    int const expectedHeight = window.getHeightInPoints();

    int calls                    = 0;
    bool handleWasUsable         = false;
    int width                    = 0;
    int height                   = 0;
    window.signalWindowDestroyed = [&]() {
        ++calls;
        // Renderer-backed textures are still valid here: the SDL window has
        // not been destroyed yet.
        handleWasUsable = (window.getSDLWindow() == handle) && SDL_GetWindowSize(handle, &width, &height);
    };

    window.destroy();

    CHECK(calls == 1);
    CHECK(handleWasUsable);
    CHECK(width == expectedWidth);
    CHECK(height == expectedHeight);
    CHECK(window.getSDLWindow() == nullptr);
}

// ---------------------------------------------------------------------------
// Settings that cannot be applied to a live window
// ---------------------------------------------------------------------------

TEST_CASE("WindowSettingsRequireRestart_DisplayAndOpenGL")
{
    WindowSettings const current;
    WindowSettings requested = current;
    CHECK_FALSE(windowSettingsRequireRestart(current, requested));

    // Everything that can be applied to the live window.
    requested.width       = 800;
    requested.height      = 600;
    requested.x           = 10;
    requested.y           = 20;
    requested.windowMode  = WindowMode::Fullscreen;
    requested.vsync       = false;
    requested.resizable   = false;
    requested.scalingMode = ScalingMode::Integer;
    CHECK_FALSE(windowSettingsRequireRestart(current, requested));

    // These need a new renderer / window, i.e. a restart.
    requested.display = 1;
    CHECK(windowSettingsRequireRestart(current, requested));

    requested        = current;
    requested.opengl = false;
    CHECK(windowSettingsRequireRestart(current, requested));
}
