# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife import fife


def test_model(engine_minimized):
    model = engine_minimized.getModel()
    map1 = model.createMap("map001")
    map2 = model.createMap("map002")

    query = model.getMap("map001")
    assert map1.getId() == query.getId()

    query = model.getMap("map002")
    assert map2.getId() == query.getId()

    query = model.getMaps()
    assert len(query) == 2

    assert len(query) == model.getMapCount()

    model.deleteMap(map2)

    query = model.getMaps()
    assert len(query) == 1

    model.createMap("map003")
    model.createMap("map004")

    query = model.getMaps()
    assert len(query) == 3
    assert model.getMapCount() == 3

    model.deleteMaps()
    query = model.getMaps()
    assert len(query) == 0
    assert model.getMapCount() == 0


def test_maps(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("map005")

    assert map_obj.getLayerCount() == 0

    grid = model.getCellGrid("square")

    layer1 = map_obj.createLayer("layer001", grid)
    layer2 = map_obj.createLayer("layer002", grid)

    assert layer1.getId() == "layer001"
    assert layer2.getId() == "layer002"

    assert len(map_obj.getLayers()) == 2

    assert map_obj.getLayerCount() == 2
    map_obj.deleteLayer(layer2)
    assert map_obj.getLayerCount() == 1
    map_obj.deleteLayers()
    assert map_obj.getLayerCount() == 0


def test_layers(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("map006")

    grid = model.getCellGrid("square")
    obj1 = model.createObject("object001", "test_nspace")
    obj2 = model.createObject("object002", "test_nspace")

    assert obj1.getId() == "object001"
    assert obj2.getId() == "object002"

    layer = map_obj.createLayer("layer003", grid)

    assert layer.hasInstances() == 0
    assert layer.getMap().getId() == map_obj.getId()

    inst = layer.createInstance(obj1, fife.ModelCoordinate(4, 4))
    layer.createInstance(obj2, fife.ModelCoordinate(5, 6))
    layer.createInstance(obj2, fife.ModelCoordinate(5, 4))

    assert len(layer.getInstances()) == 3
    assert inst.getLocation().getLayerCoordinates() == fife.ModelCoordinate(4, 4)


def test_objects(engine_minimized):
    model = engine_minimized.getModel()
    obj1 = model.createObject("object003", "test_nspace")
    obj2 = model.createObject("object004", "test_nspace")
    model.createObject("object005", "test_nspace")
    model.createObject("object006", "test_nspace")

    assert model.deleteObject(obj2) is True

    map_obj = model.createMap("map007")
    grid = model.getCellGrid("square")
    layer = map_obj.createLayer("layer004", grid)

    inst = layer.createInstance(obj1, fife.ModelCoordinate(4, 4))
    assert model.deleteObject(obj1) is False
    assert model.deleteObjects() is False
    layer.deleteInstance(inst)
    assert model.deleteObject(obj1) is True
    assert model.deleteObjects() is True


@pytest.fixture
def action_angles_engine(engine_minimized):
    anim0 = engine_minimized.getAnimationManager().create("0")
    anim1 = engine_minimized.getAnimationManager().create("1")
    anim2 = engine_minimized.getAnimationManager().create("2")
    anim3 = engine_minimized.getAnimationManager().create("3")
    runaction = fife.Action("action001")
    fife.ActionVisual.create(runaction)
    runaction.get2dGfxVisual().addAnimation(90, anim1)
    runaction.get2dGfxVisual().addAnimation(0, anim0)
    runaction.get2dGfxVisual().addAnimation(270, anim3)
    runaction.get2dGfxVisual().addAnimation(180, anim2)
    walkaction = fife.Action("action002")
    fife.ActionVisual.create(walkaction)
    walkaction.get2dGfxVisual().addAnimation(70, anim1)
    walkaction.get2dGfxVisual().addAnimation(200, anim2)
    walkaction.get2dGfxVisual().addAnimation(320, anim3)
    return {
        "engine": engine_minimized,
        "anim0": anim0,
        "anim1": anim1,
        "anim2": anim2,
        "anim3": anim3,
        "runaction": runaction,
        "walkaction": walkaction,
    }


def test_run_angle_89(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(89)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_run_angle_90(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(90)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_run_angle_91(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(91)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_run_angle_135(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(135)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_run_angle_134(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(134)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_run_angle_136(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(136)
        .getHandle()
        == action_angles_engine["anim2"].getHandle()
    )


def test_run_angle_0(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(0)
        .getHandle()
        == action_angles_engine["anim0"].getHandle()
    )


def test_run_angle_40(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(40)
        .getHandle()
        == action_angles_engine["anim0"].getHandle()
    )


def test_run_angle_45(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(45)
        .getHandle()
        == action_angles_engine["anim0"].getHandle()
    )


def test_run_angle_270(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(270)
        .getHandle()
        == action_angles_engine["anim3"].getHandle()
    )


def test_run_angle_269(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(269)
        .getHandle()
        == action_angles_engine["anim3"].getHandle()
    )


def test_run_angle_271(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(271)
        .getHandle()
        == action_angles_engine["anim3"].getHandle()
    )


def test_run_angle_314(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(314)
        .getHandle()
        == action_angles_engine["anim3"].getHandle()
    )


def test_run_angle_359(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(359)
        .getHandle()
        == action_angles_engine["anim0"].getHandle()
    )


def test_run_angle_400(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(400)
        .getHandle()
        == action_angles_engine["anim0"].getHandle()
    )


def test_run_angle_451(action_angles_engine):
    assert (
        action_angles_engine["runaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(451)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_walk_angle_0(action_angles_engine):
    assert (
        action_angles_engine["walkaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(0)
        .getHandle()
        == action_angles_engine["anim3"].getHandle()
    )


def test_walk_angle_60(action_angles_engine):
    assert (
        action_angles_engine["walkaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(60)
        .getHandle()
        == action_angles_engine["anim1"].getHandle()
    )


def test_walk_angle_199(action_angles_engine):
    assert (
        action_angles_engine["walkaction"]
        .get2dGfxVisual()
        .getAnimationByAngle(199)
        .getHandle()
        == action_angles_engine["anim2"].getHandle()
    )


def test_hex_grid(engine_minimized):
    model = engine_minimized.getModel()
    grid = model.getCellGrid("hexagonal")
    curpos = (1, 1)
    access = {
        (0, 0): False,
        (0, 1): True,
        (0, 2): False,
        (1, 0): True,
        (1, 1): True,
        (1, 2): True,
        (2, 0): True,
        (2, 1): True,
        (2, 2): True,
    }
    cost = {
        (0, 1): 1,
        (1, 0): 1,
        (1, 1): 0,
        (1, 2): 1,
        (2, 0): 1,
        (2, 1): 1,
        (2, 2): 1,
    }

    for k, v in access.items():
        assert (
            grid.isAccessible(fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k))
            == v
        )
    for k, v in cost.items():
        assert int(
            10000
            * grid.getAdjacentCost(
                fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k)
            )
        ) == int(10000 * v)

    curpos = fife.ModelCoordinate(*curpos)
    accessiblepts = fife.ModelCoordinateVector()
    grid.getAccessibleCoordinates(curpos, accessiblepts)
    costpts = [fife.ModelCoordinate(*pt) for pt in cost.keys()]
    for pt in costpts:
        assert pt in accessiblepts
    for pt in accessiblepts:
        assert pt in costpts


def test_square_grid(engine_minimized):
    model = engine_minimized.getModel()
    grid = model.getCellGrid("square")
    grid.setAllowDiagonals(False)
    curpos = (1, 1)
    access = {
        (0, 0): False,
        (0, 1): True,
        (0, 2): False,
        (1, 0): True,
        (1, 1): True,
        (1, 2): True,
        (2, 0): False,
        (2, 1): True,
        (2, 2): False,
    }
    cost = {
        (0, 1): 1,
        (1, 0): 1,
        (1, 1): 0,
        (1, 2): 1,
        (2, 1): 1,
    }

    for k, v in access.items():
        assert (
            grid.isAccessible(fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k))
            == v
        )
    for k, v in cost.items():
        assert int(
            10000
            * grid.getAdjacentCost(
                fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k)
            )
        ) == int(10000 * v)

    curpos = fife.ModelCoordinate(*curpos)
    accessiblepts = fife.ModelCoordinateVector()
    grid.getAccessibleCoordinates(curpos, accessiblepts)
    costpts = [fife.ModelCoordinate(*pt) for pt in cost.keys()]
    for pt in costpts:
        assert pt in accessiblepts
    for pt in accessiblepts:
        assert pt in costpts


def test_diag_square_grid(engine_minimized):
    model = engine_minimized.getModel()
    grid = model.getCellGrid("square")
    grid.setAllowDiagonals(True)
    curpos = (1, 1)
    access = {
        (0, 0): True,
        (0, 1): True,
        (0, 2): True,
        (1, 0): True,
        (1, 1): True,
        (1, 2): True,
        (2, 0): True,
        (2, 1): True,
        (2, 2): True,
    }
    cost = {
        (0, 0): 1.4,
        (0, 1): 1,
        (0, 2): 1.4,
        (1, 0): 1,
        (1, 1): 0,
        (1, 2): 1,
        (2, 0): 1.4,
        (2, 1): 1,
        (2, 2): 1.4,
    }

    for k, v in access.items():
        assert (
            grid.isAccessible(fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k))
            == v
        )
    for k, v in cost.items():
        assert int(
            10000
            * grid.getAdjacentCost(
                fife.ModelCoordinate(*curpos), fife.ModelCoordinate(*k)
            )
        ) == int(10000 * v)

    curpos = fife.ModelCoordinate(*curpos)
    accessiblepts = fife.ModelCoordinateVector()
    grid.getAccessibleCoordinates(curpos, accessiblepts)
    costpts = [fife.ModelCoordinate(*pt) for pt in cost.keys()]
    for pt in costpts:
        assert pt in accessiblepts
    for pt in accessiblepts:
        assert pt in costpts
