# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import gc
import os

from fife import fife


def test_camera(engine):
    model = engine.getModel()
    map_obj = model.createMap("map001")
    grid = model.getCellGrid("square")

    img_mgr = engine.getImageManager()
    anim_mgr = engine.getAnimationManager()

    frame_delay = 100

    anim = anim_mgr.create("crate")

    imgs = []
    imgs.append(img_mgr.load("tests/data/crate/full_s_000.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0001.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0002.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0003.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0004.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0005.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0006.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0007.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0008.png"))

    for img in imgs:
        anim.addFrame(img, frame_delay)

    obj = model.createObject("0", "test_nspace")
    fife.ObjectVisual.create(obj)
    img = img_mgr.get("tests/data/earth_1.png")
    obj.get2dGfxVisual().addStaticImage(0, img.getHandle())

    screen_cell_w = img.getWidth()
    screen_cell_h = img.getHeight()

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

    genericrenderer = fife.GenericRenderer.getInstance(cam)
    genericrenderer.addActiveLayer(layer)
    genericrenderer.setEnabled(True)

    engine.initializePumping()

    for y in range(4):
        for x in range(4):
            i = layer.createInstance(obj, fife.ModelCoordinate(x, y))
            fife.InstanceVisual.create(i)
            engine.pump()

    i = layer.createInstance(obj, fife.ModelCoordinate(0, 0))
    fife.InstanceVisual.create(i)
    node = fife.RendererNode(i)
    genericrenderer.addAnimation("test", node, anim)

    i = layer.createInstance(obj, fife.ModelCoordinate(2, 0))
    fife.InstanceVisual.create(i)
    node = fife.RendererNode(i)
    genericrenderer.addAnimation("test", node, anim)

    max_pumps = int(os.environ.get("FIFE_SWIG_ANIMATION_TEST_PUMPS", "80"))
    for _ in range(max_pumps):
        engine.pump()

    engine.finalizePumping()

    gc.collect()
