# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife


def test_pure_point_overlay_drawn_on_several_layers(engine_minimized):
    engine = engine_minimized
    model = engine.getModel()
    map_obj = model.createMap("rn_map")
    grid = model.getCellGrid("square")

    rb = engine.getRenderBackend()
    viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())
    cam = map_obj.addCamera("rn_cam", viewport)
    cam.setCellImageDimensions(20, 20)

    layer1 = map_obj.createLayer("rn_layer1", grid)
    layer2 = map_obj.createLayer("rn_layer2", grid)
    cam.setLocation(fife.Location(layer1))

    genericrenderer = fife.GenericRenderer.getInstance(cam)
    genericrenderer.addActiveLayer(layer1)
    genericrenderer.addActiveLayer(layer2)
    genericrenderer.setEnabled(True)

    # Pure-point overlay: no instance, no relative location, no attached layer.
    node = fife.RendererNode(fife.Point(12, 12))

    # getCalculatedPoint() must not mutate the node: no fallback attach (stage 1).
    calc = node.getCalculatedPoint(cam, layer1, False)
    assert calc.x == 12 and calc.y == 12
    assert node.getLayer() is None

    # Engine guard contract: the pure point is drawn on every active layer (variant E).
    assert node.isDrawnOn(layer1)
    assert node.isDrawnOn(layer2)

    # Translucent (alpha=128) pure-point overlay, added on both active layers.
    genericrenderer.addPoint("rn_overlay", node, 200, 100, 50, 128)

    engine.initializePumping()
    for _ in range(5):
        engine.pump()
