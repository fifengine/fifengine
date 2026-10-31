# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Regression test for the display fallback in ``Engine::init()``.

When the configured display index does not exist (monitor detached, settings
copied from another machine), the engine falls back to display 0. The fallback
must be published through ``EngineSettings`` as well, otherwise the settings
dialog and the next launch keep using the invalid index.

Part of the unknown-horizons#2923 investigation (settings must describe the
window that actually exists).
"""

import pytest

from fife import fife


@pytest.fixture(scope="module")
def engine_with_invalid_display():
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    settings.setScreenWidth(320)
    settings.setScreenHeight(200)
    settings.setDefaultFontPath("../data/FreeMono.ttf")
    # Only one display exists under the dummy/SDL test driver.
    settings.setDisplay(99)
    engine.init()
    yield engine
    engine.destroy()


def test_out_of_range_display_falls_back_and_is_published(engine_with_invalid_display):
    engine = engine_with_invalid_display
    window_display = engine.getWindow().getSettings().display

    assert window_display == 0
    assert engine.getSettings().getDisplay() == window_display
