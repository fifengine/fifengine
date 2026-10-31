# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""GUI scaling tests for the Fifechan manager (unknown-horizons#2923, WP-05).

These tests run headless. The dummy SDL driver reports a display scale of
1.0, so the engine-driven scale is identity; the manager API itself is
exercised with explicit values.
"""

import pytest

from fife import fife

FONT_MANIFEST = """<?xml version="1.0"?>
<fonts version="1">
  <family id="ScalingTest">
    <face weight="400">tests/data/FreeMono.ttf</face>
  </family>
</fonts>"""

FONT_FAMILY = "ScalingTest"


@pytest.fixture(scope="module")
def gui(shared_engine):
    """Engine with one attached Fifechan manager and a loadable font.

    Yields:
        The shared engine and the attached manager.
    """
    engine = shared_engine
    engine.loadFontManifestFromString(FONT_MANIFEST)
    engine.getSettings().setDefaultFontId(FONT_FAMILY)

    manager = fife.FifechanManager()
    # Engine::setGuiManager takes ownership through unique_ptr.
    manager.thisown = 0
    engine.setGuiManager(manager)

    backend = engine.getRenderBackend()
    manager.init(backend.getName(), backend.getScreenWidth(), backend.getScreenHeight())

    yield engine, manager

    manager.setDPIScale(1.0)


def test_gui_scaling_is_enabled_by_default(gui):
    engine, manager = gui

    assert engine.getSettings().isGUIScalingEnabled() is True
    # The dummy driver has no OS scaling, so the engine applies identity.
    assert manager.getDPIScale() == pytest.approx(1.0)
    assert engine.getWindow().getDisplayScale() > 0.0


def test_set_dpi_scale_roundtrip_and_font_height(gui):
    _, manager = gui
    font = manager.getDefaultFont()
    assert font is not None

    manager.setDPIScale(1.0)
    base_height = font.getHeight()
    assert base_height > 0

    manager.setDPIScale(1.5)
    assert manager.getDPIScale() == pytest.approx(1.5)
    scaled_height = font.getHeight()
    assert scaled_height > base_height

    # Re-applying the same scale must not compound the font size.
    manager.setDPIScale(1.5)
    assert font.getHeight() == scaled_height

    # Scaling back restores the original rasterization size.
    manager.setDPIScale(1.0)
    assert manager.getDPIScale() == pytest.approx(1.0)
    assert font.getHeight() == base_height


def test_non_positive_scale_falls_back_to_identity(gui):
    _, manager = gui

    manager.setDPIScale(2.0)
    assert manager.getDPIScale() == pytest.approx(2.0)

    manager.setDPIScale(0.0)
    assert manager.getDPIScale() == pytest.approx(1.0)

    manager.setDPIScale(-4.0)
    assert manager.getDPIScale() == pytest.approx(1.0)

    # Direct API calls use the same safe upper bound as the engine path.
    manager.setDPIScale(100.0)
    assert manager.getDPIScale() == pytest.approx(8.0)
    manager.setDPIScale(1.0)
