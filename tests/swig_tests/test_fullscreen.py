# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Reproduction tests for unknown-horizons/unknown-horizons#2923.

The issue reports that choosing 1920x1080 + fullscreen on Windows produces a
zoomed / cropped UI.  The engine-side root causes covered here are:

* ``EngineSettings`` keeps the *requested* size/fullscreen flag and is never
  updated to the size of the window that actually exists (DPI scaling,
  clamping, fullscreen entry, user resize).
* ``Engine.toggleFullscreen()`` / ``Engine.changeWindowMode()`` replace the
  ``SDL_Window`` while the render backend keeps a reference to the old one.

These tests are expected to fail on the unfixed engine and pass afterwards.
They are written against the public Python API on purpose: that is the API
Unknown Horizons uses (``horizons/view.py`` sizes its camera viewport from
``engine_settings.getScreenWidth()``).
"""

import pytest

from fife import fife

_WINDOWED = fife.WindowMode_Windowed
_FULLSCREEN = fife.WindowMode_Fullscreen


@pytest.fixture(scope="module")
def engine(shared_engine):
    """One engine for the whole module (only one Engine may be alive)."""
    shared_engine.initializePumping()
    yield shared_engine
    try:
        shared_engine.finalizePumping()
    except Exception:  # pragma: no cover - teardown should not mask failures
        pass


def _pump(engine, count=2):
    for _ in range(count):
        engine.pump()



def _copy_settings(settings):
    """Return an independent WindowSettings copy.

    ``Window.getSettings()`` returns a reference to the live settings, so
    mutating the returned proxy would modify the window itself.
    """
    copy = fife.WindowSettings()
    for field in (
        "width",
        "height",
        "display",
        "x",
        "y",
        "title",
        "icon",
        "resizable",
        "vsync",
        "opengl",
        "windowMode",
        "scalingMode",
    ):
        setattr(copy, field, getattr(settings, field))
    return copy


def _window_size(window):
    return (window.getWidthInPoints(), window.getHeightInPoints())


# ---------------------------------------------------------------------------
# Settings <-> window synchronisation
# ---------------------------------------------------------------------------


def test_initial_settings_match_window(engine):
    """After init the engine settings must describe the real window."""
    settings = engine.getSettings()
    window = engine.getWindow()
    _pump(engine)

    assert settings.getScreenWidth() == window.getWidthInPoints()
    assert settings.getScreenHeight() == window.getHeightInPoints()
    assert settings.isFullScreen() is (window.getFullscreenMode() == _FULLSCREEN)


def test_toggle_fullscreen_updates_engine_settings(engine):
    """F11 / Engine.toggleFullscreen() must update EngineSettings.isFullScreen().

    Fails on the unfixed engine: the window switches to fullscreen but the
    settings keep reporting windowed mode, so an application that reads the
    settings after the toggle keeps rendering at the old size.
    """
    settings = engine.getSettings()
    window = engine.getWindow()

    window.setFullscreenMode(_WINDOWED)
    _pump(engine)
    assert settings.isFullScreen() is False

    engine.toggleFullscreen()
    _pump(engine)

    assert window.getFullscreenMode() == _FULLSCREEN
    assert settings.isFullScreen() is True, "EngineSettings did not follow the fullscreen toggle"

    engine.toggleFullscreen()
    _pump(engine)

    assert window.getFullscreenMode() == _WINDOWED
    assert settings.isFullScreen() is False


def test_toggle_fullscreen_roundtrip_restores_window(engine):
    """A fullscreen round-trip must restore the previous windowed size."""
    window = engine.getWindow()
    window.setFullscreenMode(_WINDOWED)
    _pump(engine)
    before = _window_size(window)

    engine.toggleFullscreen()
    _pump(engine)
    engine.toggleFullscreen()
    _pump(engine)

    assert _window_size(window) == before


def test_repeated_toggles_keep_backend_attached(engine):
    """Ten fullscreen round-trips must leave the backend on the live window.

    The engine no longer recreates the SDL window when the mode changes.  A
    regression that starts replacing it again would leave the render backend
    pointing at the discarded window after a few toggles.
    """
    backend = engine.getRenderBackend()
    window = engine.getWindow()
    settings = engine.getSettings()
    window.setFullscreenMode(_WINDOWED)
    _pump(engine)

    for _ in range(10):
        engine.toggleFullscreen()
        _pump(engine)
        assert backend.isWindowAttached(), "render backend lost the window during a toggle"
        assert settings.isFullScreen() is (window.getFullscreenMode() == _FULLSCREEN)
        assert settings.getScreenWidth() == window.getWidthInPoints()
        assert settings.getScreenHeight() == window.getHeightInPoints()

    window.setFullscreenMode(_WINDOWED)
    _pump(engine)


def test_resize_updates_engine_settings(engine):
    """A window resize must be reflected in EngineSettings.

    Unknown Horizons sizes its camera viewport from
    ``engine_settings.getScreenWidth()/getScreenHeight()``; if those values
    stay stale the viewport is larger than the window (cropped UI).
    """
    settings = engine.getSettings()
    window = engine.getWindow()
    original = _window_size(window)

    window.setSize(800, 600)
    _pump(engine, 4)

    try:
        assert _window_size(window) == (800, 600)
        assert (settings.getScreenWidth(), settings.getScreenHeight()) == (800, 600)
    finally:
        window.setSize(*original)
        _pump(engine, 4)


# ---------------------------------------------------------------------------
# Render backend <-> window lifetime
# ---------------------------------------------------------------------------


def test_change_window_mode_keeps_render_backend_attached(engine):
    """A mode change must not leave the backend pointing at a dead window.

    ``Engine.changeWindowMode()`` used to call ``Window::create()``, which
    destroys the SDL window (and, in SDL3, its renderer) without telling the
    render backend.  ``RenderBackend.isWindowAttached()`` exposes the invariant
    the backend relies on.
    """
    backend = engine.getRenderBackend()
    assert hasattr(backend, "isWindowAttached"), "RenderBackend.isWindowAttached() diagnostic is missing"
    assert backend.isWindowAttached() is True

    window = engine.getWindow()
    settings = window.getSettings()

    # Windowed -> fullscreen -> windowed, through the public engine API.
    window.setFullscreenMode(_WINDOWED)
    _pump(engine)

    engine.toggleFullscreen()
    _pump(engine)
    assert backend.isWindowAttached() is True, "backend lost the window while entering fullscreen"

    engine.toggleFullscreen()
    _pump(engine)
    assert backend.isWindowAttached() is True, "backend lost the window while leaving fullscreen"

    # Sizes are applied to the live window, never by replacing it.
    engine.changeWindowMode(settings)
    _pump(engine)
    assert backend.isWindowAttached() is True, "changeWindowMode() replaced the window under the backend"

    # Rendering after the changes must still work (no dead SDL renderer).
    assert engine.getRenderBackend().getWidth() > 0
    assert engine.getRenderBackend().getHeight() > 0


def test_change_window_mode_applies_size_to_live_window(engine):
    """`changeWindowMode()` must resize the existing window, not swap it out."""
    backend = engine.getRenderBackend()
    window = engine.getWindow()

    window.setFullscreenMode(_WINDOWED)
    _pump(engine)

    settings = _copy_settings(window.getSettings())
    settings.windowMode = _WINDOWED
    settings.width = 720
    settings.height = 480

    engine.changeWindowMode(settings)
    _pump(engine, 4)

    try:
        assert _window_size(window) == (720, 480)
        assert (engine.getSettings().getScreenWidth(), engine.getSettings().getScreenHeight()) == (720, 480)
        assert backend.isWindowAttached() is True
    finally:
        # Restore a sane geometry for the remaining tests in this module.
        window.setSize(640, 400)
        _pump(engine, 4)


def test_window_resizable_roundtrip(engine):
    """`Window.setResizable()`/`isResizable()` are part of the new window API."""
    window = engine.getWindow()
    original = window.isResizable()

    window.setResizable(not original)
    assert window.isResizable() is (not original)

    window.setResizable(original)
    assert window.isResizable() is original


# ---------------------------------------------------------------------------
# Settings that need a restart (monitor / OpenGL)
# ---------------------------------------------------------------------------


def test_window_settings_restart_detection(engine):
    """Display and OpenGL changes cannot be applied to a live window.

    The settings dialog needs to know this before applying, otherwise it
    silently drops the monitor selection (or the OpenGL choice) and the option
    looks broken.
    """
    window = engine.getWindow()
    current = _copy_settings(window.getSettings())

    assert engine.windowSettingsRequireRestart(current) is False

    live_change = _copy_settings(current)
    live_change.width = 800
    live_change.height = 600
    live_change.windowMode = _WINDOWED
    live_change.vsync = not live_change.vsync
    live_change.resizable = not live_change.resizable
    live_change.scalingMode = fife.ScalingMode_Integer
    assert engine.windowSettingsRequireRestart(live_change) is False

    display_change = _copy_settings(current)
    display_change.display = current.display + 1
    assert engine.windowSettingsRequireRestart(display_change) is True

    opengl_change = _copy_settings(current)
    opengl_change.opengl = not current.opengl
    assert engine.windowSettingsRequireRestart(opengl_change) is True


def test_change_window_mode_ignores_display_change(engine):
    """A monitor change is refused, but the rest of the settings still apply."""
    window = engine.getWindow()
    current = _copy_settings(window.getSettings())
    original_size = _window_size(window)

    requested = _copy_settings(current)
    requested.display = current.display + 1
    requested.width = 700
    requested.height = 520
    requested.windowMode = _WINDOWED

    engine.changeWindowMode(requested)
    _pump(engine, 4)

    try:
        assert window.getSettings().display == current.display
        assert _window_size(window) == (700, 520)
    finally:
        window.setSize(*original_size)
        _pump(engine, 4)
