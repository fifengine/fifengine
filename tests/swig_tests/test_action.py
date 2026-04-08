# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from fife import fife


def test_walk_around(engine):
    model = engine.getModel()
    anim_mgr = engine.getAnimationManager()
    img_mgr = engine.getImageManager()

    map_obj = model.createMap("map001")
    grid = model.getCellGrid("square")

    layer = map_obj.createLayer("Layer001", grid)
    layer.setWalkable(True)
    layer.createCellCache()

    target = fife.Location(layer)

    obj = fife.Object("object001", "plaa")
    fife.ObjectVisual.create(obj)
    pather = fife.RoutePather()

    obj.setPather(pather)
    action = obj.createAction("walk")
    fife.ActionVisual.create(action)

    dirnames = ["e", "ne", "n", "nw", "w", "sw", "s", "se"]
    for index, direction in enumerate(dirnames):
        degree = 45 * index
        animation = anim_mgr.create("walk_" + direction)
        frame1 = img_mgr.load(f"tests/data/wolf_walk/gfx/fidgit_{direction}_1.png")
        frame2 = img_mgr.load(f"tests/data/wolf_walk/gfx/fidgit_{direction}_2.png")
        animation.addFrame(frame1, 100)
        animation.addFrame(frame2, 100)
        action.get2dGfxVisual().addAnimation(degree, animation)

    ground = fife.Object("ground", "plaa")
    image = engine.getImageManager().load("tests/data/earth_1.png")
    fife.ObjectVisual.create(ground)
    ground.get2dGfxVisual().addStaticImage(0, image.getHandle())
    ground.img = engine.getImageManager().get(image.getHandle())

    for y in range(-2, 3):
        for x in range(-2, 3):
            inst = layer.createInstance(ground, fife.ModelCoordinate(x, y))
            fife.InstanceVisual.create(inst)

    inst = layer.createInstance(obj, fife.ModelCoordinate(-2, -2))
    fife.InstanceVisual.create(inst)

    rb = engine.getRenderBackend()
    viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

    cam = map_obj.addCamera("foo", viewport)
    cam.setCellImageDimensions(ground.img.getWidth(), ground.img.getHeight())
    cam.setRotation(45)
    cam.setTilt(40)

    renderer = fife.InstanceRenderer.getInstance(cam)
    renderer.activateAllLayers(map_obj)

    engine.initializePumping()
    target.setLayerCoordinates(fife.ModelCoordinate(2, -2))
    inst.move("walk", target, 0.9)
    targets = [
        (2, 0),
        (2, -1),
        (2, -2),
        (1, -2),
        (0, -2),
        (-1, -2),
        (-2, -2),
        (-2, -1),
        (-2, 0),
        (-2, 1),
        (-2, 2),
        (-1, 2),
        (0, 2),
        (1, 2),
        (2, 2),
        (2, 1),
    ]
    for target_coord in targets:
        location = inst.getLocation()
        location.setLayerCoordinates(fife.ModelCoordinate(0, 0))
        inst.setLocation(location)
        target.setLayerCoordinates(fife.ModelCoordinate(*target_coord))
        inst.move("walk", target, 0.9)
        for _ in range(10):
            engine.pump()

    engine.finalizePumping()
    map_obj.removeCamera("foo")

    try:
        engine.destroy()
    except Exception:
        pass
