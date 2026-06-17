# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest

from fife import fife

_WindowMode_Windowed = getattr(fife, "WindowMode_Windowed", 0)
_WindowMode_Fullscreen = getattr(fife, "WindowMode_Fullscreen", 1)
_WindowMode_Borderless = getattr(fife, "WindowMode_Borderless", 2)
_ScalingMode_Linear = getattr(fife, "ScalingMode_Linear", 0)
_ScalingMode_Integer = getattr(fife, "ScalingMode_Integer", 1)
_ScalingMode_Nearest = getattr(fife, "ScalingMode_Nearest", 2)


class TestWindowSettings:
    def test_default_values(self):
        ws = fife.WindowSettings()
        assert ws.width == 1280
        assert ws.height == 720
        assert ws.display == 0
        assert ws.x == -1
        assert ws.y == -1
        assert ws.resizable is True
        assert ws.vsync is True
        assert ws.opengl is True
        assert ws.windowMode == _WindowMode_Windowed
        assert ws.scalingMode == _ScalingMode_Linear

    def test_field_assignment(self):
        ws = fife.WindowSettings()
        ws.width = 640
        ws.height = 480
        ws.display = 1
        ws.x = 100
        ws.y = 200
        ws.title = "Test"
        ws.icon = "icon.png"
        ws.resizable = False
        ws.vsync = False
        ws.opengl = False
        ws.windowMode = _WindowMode_Fullscreen
        ws.scalingMode = _ScalingMode_Nearest
        assert ws.width == 640
        assert ws.height == 480
        assert ws.display == 1
        assert ws.x == 100
        assert ws.y == 200
        assert ws.title == "Test"
        assert ws.icon == "icon.png"
        assert ws.resizable is False
        assert ws.vsync is False
        assert ws.opengl is False
        assert ws.windowMode == _WindowMode_Fullscreen
        assert ws.scalingMode == _ScalingMode_Nearest


class TestWindowMode:
    def test_values(self):
        assert int(_WindowMode_Windowed) == 0
        assert int(_WindowMode_Fullscreen) == 1
        assert int(_WindowMode_Borderless) == 2


class TestScalingMode:
    def test_values(self):
        assert int(_ScalingMode_Linear) == 0
        assert int(_ScalingMode_Integer) == 1
        assert int(_ScalingMode_Nearest) == 2


class TestDisplay:
    def test_get_display_count(self):
        count = fife.Display.getDisplayCount()
        assert count >= 0

    def test_get_display_name(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            name = fife.Display.getDisplayName(0)
            assert isinstance(name, str)
            assert len(name) > 0

    def test_get_display_content_scale(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            scale = fife.Display.getDisplayContentScale(0)
            assert scale > 0.0

    def test_get_display_width_height(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            w = fife.Display.getDisplayWidth(0)
            h = fife.Display.getDisplayHeight(0)
            assert w > 0
            assert h > 0

    def test_get_display_bounds(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            bounds = fife.Display.getDisplayBounds(0)
            assert len(bounds) == 4

    def test_get_usable_bounds(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            bounds = fife.Display.getUsableBounds(0)
            assert len(bounds) == 4


@pytest.mark.skipif("not hasattr(fife.Display, 'getDesktopMode')")
class TestDisplayAdvanced:
    def test_get_desktop_mode(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            mode = fife.Display.getDesktopMode(0)
            assert len(mode) == 4

    def test_get_supported_modes(self):
        count = fife.Display.getDisplayCount()
        if count > 0:
            modes = fife.Display.getSupportedModes(0)
            assert isinstance(modes, (list, tuple))


class TestEngineSettingsWindowIntegration:
    def test_window_resizable_setting(self, engine_minimized):
        s = engine_minimized.getSettings()
        assert hasattr(s, "setWindowResizable")
        s.setWindowResizable(True)
        assert s.isWindowResizable() is True
        s.setWindowResizable(False)
        assert s.isWindowResizable() is False

    def test_window_scaling_mode_setting(self, engine_minimized):
        s = engine_minimized.getSettings()
        assert hasattr(s, "setWindowScalingMode")
        s.setWindowScalingMode(0)
        assert s.getWindowScalingMode() == 0
        s.setWindowScalingMode(1)
        assert s.getWindowScalingMode() == 1
        s.setWindowScalingMode(2)
        assert s.getWindowScalingMode() == 2

    def test_engine_has_window(self, engine_minimized):
        win = engine_minimized.getWindow()
        assert win is not None
        assert win.getWidthInPoints() > 0
        assert win.getHeightInPoints() > 0
        assert win.getDPIScaleFactor() >= 1.0

    def test_window_settings_sync(self, engine_minimized):
        s = engine_minimized.getSettings()
        win = engine_minimized.getWindow()
        ws = win.getSettings()
        assert ws.width > 0
        assert ws.height > 0
        assert ws.resizable == s.isWindowResizable()
