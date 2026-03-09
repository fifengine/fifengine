#!/usr/bin/env python
# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from __future__ import absolute_import, print_function

from builtins import range

from .swig_test_utils import fife, getEngine, unittest


class MyEventListener(fife.ICommandListener):
    def __init__(self):
        fife.ICommandListener.__init__(self)

    def onCommand(self, command):
        print("received command with code %d" % command.getCode())


class TestEventChannel(unittest.TestCase):

    def setUp(self):
        self.engine = getEngine(True)
        self.eventmanager = self.engine.getEventManager()

    def tearDown(self):
        self.engine.destroy()

    def testEvents(self):
        listener = MyEventListener()
        self.eventmanager.addCommandListener(listener)
        cmd = fife.Command()
        cmd.setCode(0)
        print("Sending commands...")
        for i in range(20):
            self.eventmanager.dispatchCommand(cmd)
            cmd.setCode(i)
        self.eventmanager.removeCommandListener(listener)


TEST_CLASSES = [TestEventChannel]

if __name__ == "__main__":
    unittest.main()
