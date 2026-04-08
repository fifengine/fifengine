# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife import fife


def test_basic_mapping(engine_minimized):
    model = engine_minimized.getModel()
    squaregrid1 = model.getCellGrid("square")
    squaregrid2 = model.getCellGrid("square")
    layer1 = fife.Layer("layer1", None, squaregrid1)
    layer2 = fife.Layer("layer2", None, squaregrid2)
    loc1 = fife.Location(layer1)
    loc2 = fife.Location(layer2)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    loc1.setLayerCoordinates(P(5, 5))
    pt = loc1.getLayerCoordinates()
    assert pt == P(5, 5)
    pt = loc1.getExactLayerCoordinates()
    assert pt == D(5, 5)
    pt = loc1.getMapCoordinates()
    assert pt == D(5, 5)
    pt = loc1.getLayerCoordinates(layer2)
    assert pt == P(5, 5)
    pt = loc1.getExactLayerCoordinates(layer2)
    assert pt == D(5, 5)


def test_square_grid_scale(engine_minimized):
    model = engine_minimized.getModel()
    squaregrid1 = model.getCellGrid("square")
    squaregrid2 = model.getCellGrid("square")
    layer1 = fife.Layer("layer1", None, squaregrid1)
    layer2 = fife.Layer("layer2", None, squaregrid2)
    loc1 = fife.Location(layer1)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    squaregrid2.setXScale(5)
    squaregrid2.setYScale(5)
    loc1.setLayerCoordinates(P(5, 5))
    pt = loc1.getLayerCoordinates(layer2)
    assert pt == P(1, 1)
    pt = loc1.getExactLayerCoordinates(layer1)
    assert pt == D(5, 5)


def test_square_grid_rotation(engine_minimized):
    model = engine_minimized.getModel()
    squaregrid1 = model.getCellGrid("square")
    layer1 = fife.Layer("layer1", None, squaregrid1)
    loc1 = fife.Location(layer1)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    squaregrid1.setRotation(90)
    loc1.setLayerCoordinates(P(3, 3))
    pt = loc1.getMapCoordinates()
    assert pt == D(-3, 3)


def test_square_grid_shifts(engine_minimized):
    model = engine_minimized.getModel()
    squaregrid1 = model.getCellGrid("square")
    layer1 = fife.Layer("layer1", None, squaregrid1)
    loc1 = fife.Location(layer1)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    squaregrid1.setXShift(-3)
    squaregrid1.setYShift(-3)
    loc1.setLayerCoordinates(P(3, 3))
    pt = loc1.getMapCoordinates()
    assert pt == D(0, 0)


def test_square_combinations(engine_minimized):
    model = engine_minimized.getModel()
    squaregrid1 = model.getCellGrid("square")
    layer1 = fife.Layer("layer1", None, squaregrid1)
    loc1 = fife.Location(layer1)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    NEAR_TRESHOLD = 0.001

    def is_near(x, y):
        return (y - NEAR_TRESHOLD) < x < (y + NEAR_TRESHOLD)

    squaregrid1.setXShift(2)
    squaregrid1.setYShift(2)
    squaregrid1.setRotation(90)
    squaregrid1.setXScale(5)
    squaregrid1.setYScale(5)
    loc1.setLayerCoordinates(P(1, 1))
    pt = loc1.getMapCoordinates()
    assert is_near(pt.x, -3)
    assert is_near(pt.y, 7)


def test_hex_grid_0row(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("test")
    hexgrid1 = model.getCellGrid("hexagonal")
    squaregrid1 = model.getCellGrid("square")
    hex_layer = map_obj.createLayer("hexlayer", hexgrid1)
    map_obj.createLayer("squarelayer", squaregrid1)
    loc1 = fife.Location(hex_layer)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    loc1.setMapCoordinates(D(-2, 0))
    assert loc1.getLayerCoordinates() == P(-2, 0)

    loc1.setMapCoordinates(D(-1, 0))
    assert loc1.getLayerCoordinates() == P(-1, 0)

    loc1.setMapCoordinates(D(0, 0))
    assert loc1.getLayerCoordinates() == P(0, 0)

    loc1.setMapCoordinates(D(1, 0))
    assert loc1.getLayerCoordinates() == P(1, 0)

    loc1.setMapCoordinates(D(2, 0))
    assert loc1.getLayerCoordinates() == P(2, 0)


def test_hex_grid_1row(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("test")
    hexgrid1 = model.getCellGrid("hexagonal")
    squaregrid1 = model.getCellGrid("square")
    hex_layer = map_obj.createLayer("hexlayer", hexgrid1)
    map_obj.createLayer("squarelayer", squaregrid1)
    loc1 = fife.Location(hex_layer)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    loc1.setMapCoordinates(D(-1.1, 1))
    assert loc1.getLayerCoordinates() == P(-2, 1)

    loc1.setMapCoordinates(D(-0.5, 1))
    assert loc1.getLayerCoordinates() == P(-1, 1)

    loc1.setMapCoordinates(D(-0.1, 1))
    assert loc1.getLayerCoordinates() == P(-1, 1)

    loc1.setMapCoordinates(D(0.1, 1))
    assert loc1.getLayerCoordinates() == P(0, 1)


def test_hex_grid_m1row(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("test")
    hexgrid1 = model.getCellGrid("hexagonal")
    squaregrid1 = model.getCellGrid("square")
    hex_layer = map_obj.createLayer("hexlayer", hexgrid1)
    map_obj.createLayer("squarelayer", squaregrid1)
    loc1 = fife.Location(hex_layer)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    loc1.setMapCoordinates(D(-1.1, -1))
    assert loc1.getLayerCoordinates() == P(-2, -1)

    loc1.setMapCoordinates(D(-0.5, -1))
    assert loc1.getLayerCoordinates() == P(-1, -1)

    loc1.setMapCoordinates(D(-0.1, -1))
    assert loc1.getLayerCoordinates() == P(-1, -1)

    loc1.setMapCoordinates(D(0.1, -1))
    assert loc1.getLayerCoordinates() == P(0, -1)


def test_edge_hits(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("test")
    hexgrid1 = model.getCellGrid("hexagonal")
    squaregrid1 = model.getCellGrid("square")
    hex_layer = map_obj.createLayer("hexlayer", hexgrid1)
    map_obj.createLayer("squarelayer", squaregrid1)
    loc1 = fife.Location(hex_layer)

    P = fife.ModelCoordinate
    D = fife.ExactModelCoordinate

    loc1.setMapCoordinates(D(0.5, 0.5))
    assert loc1.getLayerCoordinates() == P(0, 1)

    loc1.setMapCoordinates(D(0.1, 0.4))
    assert loc1.getLayerCoordinates() == P(0, 0)

    loc1.setMapCoordinates(D(0.1, -0.4))
    assert loc1.getLayerCoordinates() == P(0, 0)

    loc1.setMapCoordinates(D(-0.5, -0.5))
    assert loc1.getLayerCoordinates() == P(-1, -1)
