#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from .swig_test_utils import fife, getEngine, unittest


class TestImgMgr(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine()

    def tearDown(self):
        self.engine.destroy()

    def testImageImgMgr(self):
        imgMgr = self.engine.getImageManager()
        self.assert_(imgMgr)
        self.assert_(imgMgr.getTotalResources() == 0)
        img = imgMgr.create("tests/data/beach_e1.png")
        self.assertEqual(imgMgr.getTotalResourcesLoaded(), 0)
        self.assertEqual(imgMgr.getTotalResourcesCreated(), 1)
        img = imgMgr.get(img.getHandle())
        self.assertEqual(imgMgr.getTotalResourcesLoaded(), 1)
        self.assertEqual(imgMgr.getTotalResourcesCreated(), 0)

    def testImageImgMgrFail(self):
        imgMgr = self.engine.getImageManager()
        self.assertRaises(fife.NotFound, imgMgr.load, "does_not_exist.png")


TEST_CLASSES = [TestImgMgr]

if __name__ == "__main__":
    unittest.main()