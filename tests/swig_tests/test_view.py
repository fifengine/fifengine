# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import time

from fife import fife


def test_camera(engine):
    model = engine.getModel()
    map_obj = model.createMap("map001")

    grid = model.getCellGrid("square")

    img_mgr = engine.getImageManager()

    obj1 = model.createObject("0", "test_nspace")
    fife.ObjectVisual.create(obj1)
    img1 = img_mgr.load("tests/data/mushroom_007.png")
    obj1.get2dGfxVisual().addStaticImage(0, img1.getHandle())

    obj2 = model.createObject("1", "test_nspace")
    fife.ObjectVisual.create(obj2)
    img2 = img_mgr.get("tests/data/earth_1.png")
    obj2.get2dGfxVisual().addStaticImage(0, img2.getHandle())

    screen_cell_w = img2.getWidth()
    screen_cell_h = img2.getHeight()

    layer = map_obj.createLayer("layer001", grid)

    rb = engine.getRenderBackend()
    viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

    cam = map_obj.addCamera("foo", viewport)
    cam.setCellImageDimensions(screen_cell_w, screen_cell_h)
    cam.setRotation(45)
    cam.setTilt(40)
    cam.setLocation(fife.Location(layer))
    cam.setViewPort(viewport)

    renderer = fife.InstanceRenderer.getInstance(cam)
    renderer.activateAllLayers(map_obj)

    engine.initializePumping()

    for y in range(4):
        for x in range(4):
            i = layer.createInstance(obj2, fife.ModelCoordinate(x, y))
            fife.InstanceVisual.create(i)
            engine.pump()
            time.sleep(0.01)
    i = layer.createInstance(obj1, fife.ModelCoordinate(0, 0))
    fife.InstanceVisual.create(i)
    i = layer.createInstance(obj1, fife.ModelCoordinate(2, 1))
    fife.InstanceVisual.create(i)

    for i in range(120):
        if i > 20 and i < 30:
            cam.setRotation(cam.getRotation() + 1)
        elif i > 30 and i < 40:
            cam.setRotation(cam.getRotation() - 1)
        elif i > 40 and i < 50:
            if i % 2 == 0:
                c = cam.getLocation().getExactLayerCoordinates()
                c.x += 0.1
                cam.getLocation().setExactLayerCoordinates(c)
        elif i > 50 and i < 60:
            if i % 2 == 0:
                c = cam.getLocation().getExactLayerCoordinates()
                c.x -= 0.1
                cam.getLocation().setExactLayerCoordinates(c)
        elif i > 60 and i < 70:
            cam.setTilt(cam.getTilt() + 1)
        elif i > 70 and i < 80:
            cam.setTilt(cam.getTilt() - 1)
        elif i > 80 and i < 90:
            cam.setZoom(cam.getZoom() + 0.010)
        elif i > 90 and i < 100:
            cam.setZoom(cam.getZoom() - 0.010)
        engine.pump()

    engine.finalizePumping()

    try:
        engine.destroy()
    except Exception:
        pass
