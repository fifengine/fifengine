#!/usr/bin/env python

# ####################################################################
#  Copyright (C) 2005-2019 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

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
