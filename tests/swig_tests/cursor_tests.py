#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


from .swig_test_utils import getEngine, unittest


class TestView(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine()
        self.model = self.engine.getModel()

        self.imgMgr = self.engine.getImageManager()
        self.animMgr = self.engine.getAnimationManager()

        frame_delay = 100

        # create the animation... messy I know
        self.cursor_anim = self.animMgr.create("cursor_anim")

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
            self.cursor_anim.addFrame(img, frame_delay)

        # create the drag animation... messy I know
        self.cursor_drag_anim = self.animMgr.create("cursor_drag_anim")

        imgs = []
        imgs.append(self.imgMgr.load("tests/data/wolf_walk/gfx/run_e_1.png"))
        imgs.append(self.imgMgr.load("tests/data/wolf_walk/gfx/run_e_2.png"))
        imgs.append(self.imgMgr.load("tests/data/wolf_walk/gfx/run_e_3.png"))
        imgs.append(self.imgMgr.load("tests/data/wolf_walk/gfx/run_e_4.png"))
        imgs.append(self.imgMgr.load("tests/data/wolf_walk/gfx/run_e_5.png"))

        for img in imgs:
            self.cursor_drag_anim.addFrame(img, frame_delay)

        self.cursor = self.engine.getCursor()
        self.cursor.set(self.cursor_anim)
        self.cursor.setDrag(self.cursor_drag_anim, 50, 50)

        self.engine.initializePumping()

    def tearDown(self):
        self.engine.finalizePumping()
        self.engine.destroy()

    def testAnimatedCursor(self):
        for i in range(250):
            self.engine.pump()


TEST_CLASSES = [TestView]

if __name__ == "__main__":
    unittest.main()
