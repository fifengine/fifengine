# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""The requested window size must survive Engine.init() display clamping.

``EngineSettings.getScreenWidth()`` reports the live window width after init
(kept in sync with the window), which is what viewport code needs. A settings
dialog, however, must still show the resolution the application asked for
even when the display forced a smaller window. That value is exposed as
``getRequestedScreenWidth()`` / ``getRequestedScreenHeight()``.

Part of the unknown-horizons#2923 investigation.
"""

import pytest

from fife import fife


@pytest.fixture(scope="module")
def engine_requested():
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    # Larger than the dummy display so the clamp actually runs.
    settings.setScreenWidth(1920)
    settings.setScreenHeight(1080)
    settings.setDefaultFontPath("../data/FreeMono.ttf")
    engine.init()
    yield engine
    engine.destroy()


def test_requested_resolution_is_kept_separate_from_effective(engine_requested):
    settings = engine_requested.getSettings()
    window = engine_requested.getWindow()

    assert settings.getRequestedScreenWidth() == 1920
    assert settings.getRequestedScreenHeight() == 1080

    # The effective size describes the window that actually exists.
    assert settings.getScreenWidth() == window.getWidthInPoints()
    assert settings.getScreenHeight() == window.getHeightInPoints()
    assert settings.getScreenWidth() <= settings.getRequestedScreenWidth()
    assert settings.getScreenHeight() <= settings.getRequestedScreenHeight()


def test_set_screen_width_remains_the_request_setter(engine_requested):
    settings = engine_requested.getSettings()
    original_width = settings.getRequestedScreenWidth()
    original_height = settings.getRequestedScreenHeight()

    try:
        settings.setScreenWidth(1280)
        settings.setScreenHeight(720)

        assert settings.getRequestedScreenWidth() == 1280
        assert settings.getRequestedScreenHeight() == 720
        # The setter also updates the legacy value immediately;
        # Engine.init() or changeWindowMode() publishes the effective size
        # afterwards.
        assert settings.getScreenWidth() == 1280
        assert settings.getScreenHeight() == 720
    finally:
        settings.setScreenWidth(original_width)
        settings.setScreenHeight(original_height)


def test_preferred_window_api_is_documented():
    assert "points" in fife.Window.getWidthInPoints.__doc__.lower()
    assert "points" in fife.Window.getHeightInPoints.__doc__.lower()
    assert "getWidthInPoints" in fife.Engine.getWindow.__doc__
