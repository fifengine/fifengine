# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors
"""Regression test for unknown-horizons/unknown-horizons#2958.

Starting in fullscreen with a resolution the display cannot provide used to
abort ``Engine::init()`` with ``NotSupported: Could not find a matching screen
mode for the values given!`` (thrown by ``DeviceCaps::getNearestScreenMode``).

The engine now treats the configured resolution as a request: it is clamped to
the display bounds, fullscreen is entered without selecting a video mode, and
the requested value stays available through ``getRequestedScreenWidth()`` /
``getRequestedScreenHeight()``.

One module = one process (``run_tests.py`` runs each module in its own pytest
subprocess) and one ``Engine`` may be alive at a time, so the oversized
resolution is configured before the single ``init()`` call.
"""

import os

import pytest

from fife import fife

_FONT_PATH = os.path.abspath(
    os.path.join(os.path.dirname(__file__), os.pardir, os.pardir, "data", "FreeMono.ttf")
)

# uint16_t is the API limit; this is larger than any single display, so the
# request is guaranteed to need clamping.
_REQUESTED_WIDTH = 30000
_REQUESTED_HEIGHT = 20000


@pytest.fixture(scope="module")
def engine_oversized():
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    settings.setScreenWidth(_REQUESTED_WIDTH)
    settings.setScreenHeight(_REQUESTED_HEIGHT)
    settings.setDefaultFontPath(_FONT_PATH)
    settings.setFullScreen(True)

    # The regression: this used to raise fife.NotSupported.
    engine.init()

    yield engine

    engine.destroy()


def test_oversized_fullscreen_does_not_abort_startup(engine_oversized):
    """init() in the fixture completed, so an Engine with a window exists."""
    window = engine_oversized.getWindow()

    assert window is not None
    assert window.getWidthInPoints() > 0
    assert window.getHeightInPoints() > 0


def test_requested_size_is_preserved(engine_oversized):
    settings = engine_oversized.getSettings()

    assert settings.getRequestedScreenWidth() == _REQUESTED_WIDTH
    assert settings.getRequestedScreenHeight() == _REQUESTED_HEIGHT
    # The effective size was clamped to the display, never kept at the request.
    assert settings.getScreenWidth() < _REQUESTED_WIDTH
    assert settings.getScreenHeight() < _REQUESTED_HEIGHT


def test_effective_settings_match_the_live_window(engine_oversized):
    settings = engine_oversized.getSettings()
    window = engine_oversized.getWindow()

    assert settings.getScreenWidth() == window.getWidthInPoints()
    assert settings.getScreenHeight() == window.getHeightInPoints()
    # Fullscreen is entered without a video mode change, so it must succeed
    # even though the requested resolution does not exist.
    assert settings.isFullScreen() is True
    assert window.getFullscreenMode() == fife.WindowMode_Fullscreen


def test_supported_screen_modes_never_throw(engine_oversized):
    """Enumerating modes is best effort: it may be empty (Wayland, dummy)."""
    devicecaps = fife.DeviceCaps()
    devicecaps.fillDeviceCaps()

    assert devicecaps.getDisplayCount() >= 1
    assert devicecaps.getDesktopWidth() > 0
    assert devicecaps.getDesktopHeight() > 0

    for mode in devicecaps.getSupportedScreenModes():
        assert mode.getWidth() > 0
        assert mode.getHeight() > 0
