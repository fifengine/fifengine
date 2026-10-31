# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Startup in fullscreen must publish the size the window actually has.

Entering fullscreen is asynchronous on some windowing systems (the SDL docs
recommend ``SDL_SyncWindow()`` when an immediate result is required).  The
engine used to read the window size before the fullscreen resize had been
applied, so ``EngineSettings`` reported the requested windowed size and
Unknown Horizons sized its camera from the wrong value.
"""

import pytest

from fife import fife


class _StartupListener(fife.IEngineChangeListener):
    def __init__(self):
        super().__init__()
        self.sizes = []

    def onWindowModeChanged(self, newmode):
        pass

    def onWindowResized(self, width, height):
        self.sizes.append((width, height))

    def onDPIScaleChanged(self, scale):
        pass


@pytest.fixture(scope="module")
def engine_fullscreen():
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    settings.setScreenWidth(800)
    settings.setScreenHeight(600)
    settings.setDefaultFontPath("../data/FreeMono.ttf")
    settings.setFullScreen(True)

    listener = _StartupListener()
    engine.addChangeListener(listener)
    engine.init()
    yield engine, listener
    engine.removeChangeListener(listener)
    engine.destroy()


def test_fullscreen_startup_settings_match_window(engine_fullscreen):
    engine, _listener = engine_fullscreen
    window = engine.getWindow()

    assert engine.getSettings().isFullScreen() is True
    assert engine.getSettings().getScreenWidth() == window.getWidthInPoints()
    assert engine.getSettings().getScreenHeight() == window.getHeightInPoints()


def test_fullscreen_startup_notifies_resize(engine_fullscreen):
    engine, listener = engine_fullscreen
    window = engine.getWindow()

    assert listener.sizes, "onWindowResized was not called during startup"
    assert listener.sizes[-1] == (window.getWidthInPoints(), window.getHeightInPoints())
