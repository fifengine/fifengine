# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Regression coverage for Engine singleton lifecycle in a single process.

A second live Engine (or a re-initialised one) constructs duplicate
``DynamicSingleton`` instances, which aborts in Debug builds and corrupts
singleton ownership in Release builds.  These tests exercise the shutdown and
re-initialisation paths directly, without relying on process isolation.
"""

import gc
import os

from fife import fife

os.environ.setdefault("SDL_VIDEODRIVER", "dummy")
os.environ.setdefault("SDL_AUDIODRIVER", "dummy")
os.environ.setdefault("ALSOFT_DRIVERS", "null")


def _make_engine():
    engine = fife.Engine()
    settings = engine.getSettings()
    settings.setRenderBackend("SDL")
    settings.setFullScreen(False)
    settings.setScreenWidth(1)
    settings.setScreenHeight(1)
    engine.init()
    return engine


def test_destroy_then_reinit_same_engine():
    """An Engine can be re-initialised after a completed shutdown."""
    engine = _make_engine()
    engine.destroy()
    engine.init()
    engine.destroy()


def test_two_engines_in_same_process():
    """Sequential Engine lifetimes do not leak C++ singleton instances."""
    first = _make_engine()
    first.destroy()

    second = _make_engine()
    second.destroy()

    del first, second
    gc.collect()


def test_destroy_is_idempotent():
    """Calling destroy() twice is safe."""
    engine = _make_engine()
    engine.destroy()
    engine.destroy()
