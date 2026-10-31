# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Engine change-listener callbacks.

``IEngineChangeListener`` is how applications (Unknown Horizons, see WP-08)
observe window size/mode/DPI changes.  ``onWindowResized`` exists in the C++
interface (``engine.h``) but must also be declared in the SWIG director
interface (``engine.i``), otherwise Python overrides are silently ignored.
"""

import contextlib

import pytest

from fife import fife

_WINDOWED = fife.WindowMode_Windowed


class _RecordingListener(fife.IEngineChangeListener):
    def __init__(self):
        super().__init__()
        self.resized = []
        self.modes = []

    def onWindowModeChanged(self, newmode):
        self.modes.append(newmode.windowMode)

    def onWindowResized(self, width, height):
        self.resized.append((width, height))

    def onDPIScaleChanged(self, scale):
        pass


@pytest.fixture(scope="module")
def engine(shared_engine):
    shared_engine.initializePumping()
    yield shared_engine
    with contextlib.suppress(Exception):
        shared_engine.finalizePumping()


def test_resize_notifies_listener(engine):
    listener = _RecordingListener()
    engine.addChangeListener(listener)
    window = engine.getWindow()
    original = (window.getWidthInPoints(), window.getHeightInPoints())
    try:
        window.setSize(900, 700)
        assert listener.resized, "onWindowResized was not called"
        assert listener.resized[-1] == (900, 700)
    finally:
        window.setSize(*original)
        engine.removeChangeListener(listener)


def test_mode_change_notifies_listener(engine):
    listener = _RecordingListener()
    engine.addChangeListener(listener)
    window = engine.getWindow()
    try:
        engine.toggleFullscreen()
        assert listener.modes, "onWindowModeChanged was not called"
        assert window.getFullscreenMode() != _WINDOWED
    finally:
        if window.getFullscreenMode() != _WINDOWED:
            engine.toggleFullscreen()
        engine.removeChangeListener(listener)
