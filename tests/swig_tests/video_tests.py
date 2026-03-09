#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import

from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class TestVideo(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine()

    def tearDown(self):
        self.engine.destroy()

    def testDrawLine(self):
        points = (
            (1, 1),
            (50, 20),
            (20, 50),
            (200, 500),
            (500, 200),
            (100, 200),
            (10, 10),
            (70, 40),
            (80, 30),
            (300, 520),
            (340, 220),
            (170, 600),
        )
        self.engine.initializePumping()

        renderer = self.engine.getOffRenderer()
        renderer.setEnabled(True)

        prevPt = fife.Point(*points[1])
        for pt in points[1:]:
            curPt = fife.Point(*pt)
            renderer.addLine("lines", prevPt, curPt, 255, 255, 255)
            prevPt = curPt

        for i in range(200):
            self.engine.pump()

        renderer.removeAll("lines")

        self.engine.finalizePumping()


TEST_CLASSES = [TestVideo]

if __name__ == "__main__":
    unittest.main()
