#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

import gc
import os
from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class TestView(unittest.TestCase):

    def setUp(self):
        self._pumping_initialized = False
        self.engine = getEngine()
        self.model = self.engine.getModel()
        self.map = self.model.createMap("map001")

        self.grid = self.model.getCellGrid("square")

        self.imgMgr = self.engine.getImageManager()
        self.animMgr = self.engine.getAnimationManager()

        frame_delay = 100

        # create the animation... messy I know
        self.anim = self.animMgr.create("crate")

        imgs = []
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_000.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0001.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0002.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0003.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0004.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0005.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0006.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0007.png"))
        imgs.append(self.imgMgr.load("tests/data/crate/full_s_0008.png"))

        for img in imgs:
            self.anim.addFrame(img, frame_delay)

        self.obj = self.model.createObject("0", "test_nspace")
        fife.ObjectVisual.create(self.obj)
        img = self.imgMgr.get("tests/data/earth_1.png")
        self.obj.get2dGfxVisual().addStaticImage(0, img.getHandle())

        self.screen_cell_w = img.getWidth()
        self.screen_cell_h = img.getHeight()

        self.layer = self.map.createLayer("layer001", self.grid)

        self.camcoord = fife.ExactModelCoordinate(2, 0)

    def tearDown(self):
        try:
            if self._pumping_initialized:
                self.engine.finalizePumping()
        except Exception:
            pass

        self.camcoord = None
        self.layer = None
        self.obj = None
        self.anim = None
        self.animMgr = None
        self.imgMgr = None
        self.grid = None
        self.map = None
        self.model = None

        if hasattr(self, "engine"):
            self.engine.destroy()
            self.engine = None

        gc.collect()

    def testCamera(self):
        rb = self.engine.getRenderBackend()
        viewport = fife.Rect(0, 0, rb.getWidth(), rb.getHeight())

        cam = self.map.addCamera("foo", viewport)
        cam.setCellImageDimensions(self.screen_cell_w, self.screen_cell_h)
        cam.setRotation(45)
        cam.setTilt(40)
        cam.setLocation(fife.Location(self.layer))

        cam.setViewPort(viewport)

        renderer = fife.InstanceRenderer.getInstance(cam)
        renderer.activateAllLayers(self.map)

        genericrenderer = fife.GenericRenderer.getInstance(cam)
        genericrenderer.addActiveLayer(self.layer)
        genericrenderer.setEnabled(True)

        self.engine.initializePumping()
        self._pumping_initialized = True

        for y in range(4):
            for x in range(4):
                i = self.layer.createInstance(self.obj, fife.ModelCoordinate(x, y))
                fife.InstanceVisual.create(i)
                self.engine.pump()

        i = self.layer.createInstance(self.obj, fife.ModelCoordinate(0, 0))
        fife.InstanceVisual.create(i)
        node = fife.RendererNode(i)
        genericrenderer.addAnimation("test", node, self.anim)

        i = self.layer.createInstance(self.obj, fife.ModelCoordinate(2, 0))
        fife.InstanceVisual.create(i)
        node = fife.RendererNode(i)
        genericrenderer.addAnimation("test", node, self.anim)

        max_pumps = int(os.environ.get("FIFE_SWIG_ANIMATION_TEST_PUMPS", "80"))
        for i in range(max_pumps):
            self.engine.pump()
        self.engine.finalizePumping()
        self._pumping_initialized = False


TEST_CLASSES = [TestView]

if __name__ == "__main__":
    unittest.main()
