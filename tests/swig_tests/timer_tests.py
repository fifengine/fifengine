#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import, print_function

import time
from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class MyTimeEvent(fife.TimeEvent):
    def __init__(self, period):
        fife.TimeEvent.__init__(self, period)
        self.counter = 0

    def updateEvent(self, curtime):
        print("testing timer event... %d, %d" % (curtime, self.counter))
        self.counter += 1


class TestTimer(unittest.TestCase):
    def setUp(self):
        self.engine = getEngine(True)
        self.timemanager = self.engine.getTimeManager()

    def tearDown(self):
        self.engine.destroy()

    def testEvents(self):
        e = MyTimeEvent(100)
        self.timemanager.registerEvent(e)

        for i in range(10):
            time.sleep(0.1)
            self.timemanager.update()

        self.timemanager.unregisterEvent(e)


TEST_CLASSES = [TestTimer]

if __name__ == "__main__":
    unittest.main()