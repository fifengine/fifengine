#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class ActionTests(unittest.TestCase):
    def setUp(self):
        self._pumping_initialized = False
        dirnames = ["e", "ne", "n", "nw", "w", "sw", "s", "se"]

        self.engine = getEngine()
        self.animMgr = self.engine.getAnimationManager()
        self.imgMgr = self.engine.getImageManager()

        self.map = self.engine.getModel().createMap("map001")
        self.grid = self.engine.getModel().getCellGrid("square")

        self.layer = self.map.createLayer("Layer001", self.grid)
        self.layer.setWalkable(True)
        self.layer.createCellCache()

        self.target = fife.Location(self.layer)

        self.obj = fife.Object("object001", "plaa")
        fife.ObjectVisual.create(self.obj)
        self.pather = fife.RoutePather()

        self.obj.setPather(self.pather)
        self.action = self.obj.createAction("walk")
        fife.ActionVisual.create(self.action)

        for index, direction in enumerate(dirnames):
            degree = 45 * index
            animation = self.animMgr.create("walk_" + direction)
            frame1 = self.imgMgr.load(
                "tests/data/wolf_walk/gfx/fidgit_%s_1.png" % direction
            )
            frame2 = self.imgMgr.load(
                "tests/data/wolf_walk/gfx/fidgit_%s_2.png" % direction
            )
            animation.addFrame(frame1, 100)
            animation.addFrame(frame2, 100)
            self.action.get2dGfxVisual().addAnimation(degree, animation)

        self.ground = fife.Object("ground", "plaa")
        image = self.engine.getImageManager().load("tests/data/earth_1.png")
        fife.ObjectVisual.create(self.ground)
        self.ground.get2dGfxVisual().addStaticImage(0, image.getHandle())
        self.ground.img = self.engine.getImageManager().get(image.getHandle())

        for y in range(-2, 3):
            for x in range(-2, 3):
                inst = self.layer.createInstance(self.ground, fife.ModelCoordinate(x, y))
                fife.InstanceVisual.create(inst)

        self.inst = self.layer.createInstance(self.obj, fife.ModelCoordinate(-2, -2))
        fife.InstanceVisual.create(self.inst)

    def tearDown(self):
        try:
            if self._pumping_initialized:
                self.engine.finalizePumping()
        except Exception:
            pass
        if hasattr(self, "engine"):
            self.engine.destroy()

    def _testWalkingAction(self):

        self.inst.move("walk", self.target, 0.05)
        self.engine.initializePumping()
        self._pumping_initialized = True
        backend = self.engine.renderBackend
        self.assertTrue(backend)
        for i in range(360):
            self.inst.getLocation().getLayerCoordinates()
            self.target.getLayerCoordinates()
            if (
                self.inst.getLocation().getLayerCoordinates()
                == self.target.getLayerCoordinates()
            ):
                break
            self.inst.update()
            action = self.inst.currentAction
            angle = 0  # self.inst.orientation
            animation = action.getAnimationByAngle(angle)
            timestamp = self.inst.actionRuntime % animation.duration
            image = animation.getFrameByTimestamp(timestamp)
            if image:
                image.render(fife.Rect(0, 0, image.width, image.height), 255)
            self.engine.pump()
        self.engine.finalizePumping()
        self._pumping_initialized = False

    def testWalkAround(self):

        rb = self.engine.getRenderBackend()
        viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

        cam = self.map.addCamera("foo", viewport)
        cam.setCellImageDimensions(
            self.ground.img.getWidth(), self.ground.img.getHeight()
        )
        cam.setRotation(45)
        cam.setTilt(40)

        renderer = fife.InstanceRenderer.getInstance(cam)
        renderer.activateAllLayers(self.map)

        self.engine.initializePumping()
        self._pumping_initialized = True
        self.target.setLayerCoordinates(fife.ModelCoordinate(2, -2))
        self.inst.move("walk", self.target, 0.9)
        targets = (
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
        )
        for target in targets:
            location = self.inst.getLocation()
            location.setLayerCoordinates(fife.ModelCoordinate(0, 0))
            self.inst.setLocation(location)
            self.target.setLayerCoordinates(fife.ModelCoordinate(*target))
            self.inst.move("walk", self.target, 0.9)
            for i in range(10):
                self.engine.pump()

        self.engine.finalizePumping()
        self._pumping_initialized = False
        self.map.removeCamera("foo")


TEST_CLASSES = [ActionTests]

if __name__ == "__main__":
    unittest.main()
