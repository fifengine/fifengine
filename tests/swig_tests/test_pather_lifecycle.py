# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Regression coverage for pather lifetime during model/engine teardown.

Actions hold a non-owning pointer to their pather.  If the pather proxy dies
before the engine that owns the active action, engine teardown must not
dereference the freed pather.
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
    settings.setScreenWidth(64)
    settings.setScreenHeight(64)
    engine.init()
    return engine


def _start_walk(engine, pather):
    """Create a map with an in-flight walk action that uses ``pather``."""
    model = engine.getModel()
    map_obj = model.createMap("map001")
    grid = model.getCellGrid("square")
    layer = map_obj.createLayer("layer001", grid)
    layer.setWalkable(True)
    layer.createCellCache()

    ground = fife.Object("ground", "plaa")
    fife.ObjectVisual.create(ground)
    for y in range(-2, 3):
        for x in range(-2, 3):
            cell = layer.createInstance(ground, fife.ModelCoordinate(x, y))
            fife.InstanceVisual.create(cell)

    obj = fife.Object("walker", "plaa")
    fife.ObjectVisual.create(obj)
    obj.setPather(pather)
    action = obj.createAction("walk")
    fife.ActionVisual.create(action)

    inst = layer.createInstance(obj, fife.ModelCoordinate(-2, -2))
    fife.InstanceVisual.create(inst)

    camera = map_obj.addCamera("camera", fife.Rect(0, 0, 64, 64))
    renderer = fife.InstanceRenderer.getInstance(camera)
    renderer.activateAllLayers(map_obj)

    target = fife.Location(layer)
    target.setLayerCoordinates(fife.ModelCoordinate(2, 2))
    engine.initializePumping()
    inst.move("walk", target, 0.9)
    engine.finalizePumping()


def test_python_owned_pather_can_die_before_engine():
    """A freed Python-owned pather is never dereferenced during teardown."""
    engine = _make_engine()
    pather = fife.RoutePather()
    _start_walk(engine, pather)

    del pather
    gc.collect()
    del engine
    gc.collect()


def test_model_adopted_pather_survives_engine_teardown():
    """A pather transferred to the model outlives the active action."""
    engine = _make_engine()
    pather = fife.RoutePather()
    pather.thisown = False
    engine.getModel().adoptPather(pather)
    _start_walk(engine, pather)

    del pather
    gc.collect()
    del engine
    gc.collect()
